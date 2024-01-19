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

typedef struct {
	int socket_fd;
} ClientState;

ClientState* init_client(const char *server_inet_addr) {
	ClientState *state = (ClientState*)malloc(sizeof(ClientState));
	state->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(state->socket_fd < 0) {
		fprintf(stderr, "error creating client socket fd: %s\n", strerror(errno));
		return NULL;
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
			return NULL;
		}
		memcpy((char*) &server_address.sin_addr, &addr, sizeof(addr));
	}

	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(PORT);

	if(connect(state->socket_fd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
		fprintf(stderr, "error connecting to server: %s\n", strerror(errno));
		return NULL;
	}

	return state;
}

void deinit_client(ClientState *client) {
	close(client->socket_fd);
	free(client);
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
	ClientState *client = init_client(argv[1]);
	if(client == NULL) {
		return EXIT_FAILURE;
	}

	struct timeval start_ts, n_pack_ts, first_pack_ts, current_ts;
	double bandwith = 0;
	start_ts.tv_sec = 0;
	start_ts.tv_usec = 0;
	n_pack_ts.tv_sec = 0;
	n_pack_ts.tv_usec = 0;
	first_pack_ts.tv_sec = 0;
	first_pack_ts.tv_usec = 0;
	current_ts.tv_sec = 0;
	current_ts.tv_usec = 0;

	
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

		for(int j = 0; j < 100; j += 1) {
			gettimeofday(&current_ts, NULL);
			char byte = 0;
			ssize_t bytes_read;
			send(client->socket_fd, data, bound, 0);
			bytes_read = recv(client->socket_fd, &byte, 1, 0);
			if(!got_nth_packet) {
				gettimeofday(&n_pack_ts, NULL);
				got_nth_packet = true;
			} else {
				bandwith = measure_bandwith(&n_pack_ts, &current_ts, (float)bytes_read);
			}
		}
		printf("Done. Measured BW = %f MB/s\n", bandwith);
		bandwith = 0;
	}

	deinit_client(client);
	return EXIT_SUCCESS;
}
