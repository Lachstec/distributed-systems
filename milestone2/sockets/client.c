#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

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
	if(argc < 2) {
		fprintf(stderr, "usage: client <server address>\n");
		return EXIT_FAILURE;
	}
	ClientState *client = init_client(argv[1]);
	if(client == NULL) {
		return EXIT_FAILURE;
	}
	const char *message = "Test123\0";
	if(send(client->socket_fd, message, strlen(message), 0) != strlen(message)) {
		fprintf(stderr, "more bytes were sent than the message was long\n");
		return EXIT_FAILURE;
	}
	deinit_client(client);
	return EXIT_SUCCESS;
}
