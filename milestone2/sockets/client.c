#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

#include "measure.h"

#define PORT 8080

// create a socket and connect to the specified inet addr. returns the socket fd or -1 on error.
int init_client(const char *server_inet_addr) {
	int socket_fd;
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(socket_fd < 0) {
		fprintf(stderr, "error creating client socket fd: %s\n", strerror(errno));
		return -1;
	}

	struct sockaddr_in server_address;
	struct hostent *host_info; 
	unsigned long addr;

	memset(&server_address, 0, sizeof(server_address));
	addr = inet_addr(server_inet_addr);

	if(addr != INADDR_NONE) {
		memcpy((char*)&server_address.sin_addr, &addr, sizeof(addr));
	} else {
		host_info = gethostbyname(server_inet_addr);
		if(host_info == NULL) {
			fprintf(stderr, "error connecting to server: %s\n", strerror(errno));
			return -1;
		}
		memcpy((char*) &server_address.sin_addr, &addr, sizeof(addr));
	}

	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(PORT);

	if(connect(socket_fd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
		fprintf(stderr, "error connecting to server: %s\n", strerror(errno));
		return -1;
	}

	return socket_fd;
}

int main(int argc, char **argv) {

	char data[8192];
	for(int i = 0; i < 8192; i += 1) {
		data[i] = 'A' + (random() % 26);
	}

	if(argc < 2) {
		fprintf(stderr, "usage: client <server address>\n");
		return EXIT_FAILURE;
	}
	int client_fd = init_client(argv[1]);
	if(client_fd < 0) {
		return EXIT_FAILURE;
	}

	for(int i = 0; i <= 4; i += 1) {
		int bound = 0;
		if(i == 0) {
			printf("Sending 100 iterations of one byte of Data...\t");
			bound = 1;
		} else {
			printf("Sending 100 iterations of %d KiB of Data...\t", 1 << (i - 1));
			bound = 1024 * (1 << i);
		}

		bool first_packet = true;
		bool got_nth_packet = false;
		Measurement *measurement = new_measurement();

		for(int j = 0; j < 100; j += 1) {
			get_current_time(measurement);
			char byte = 0;
			ssize_t bytes_read;
			send(client_fd, data, bound, 0);
			bytes_read = recv(client_fd, &byte, 1, 0);
			if(!got_nth_packet) {
				start_measurement(measurement);
				got_nth_packet = true;
			} else {
				measure_bandwidth(measurement, bytes_read);
			}
		}
		printf("Done. Measured BW = %f MB/s\n", measurement->bandwidth);
		free_measurement(measurement);
	}

	return EXIT_SUCCESS;
}
