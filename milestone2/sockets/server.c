#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
#include <unistd.h>

#define PORT 8080
#define BUF_SIZE 1024

// Creates a ServerState by allocating memory for it and setting socket_fd to 
// a valid TCP-Socket that gets bound to localhost on PORT.
int init_server() {
	// create the socket fd
	int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	// check if socket was created successfully
	if(socket_fd < 0) {
		fprintf(stderr, "error creating a socket: %s\n", strerror(errno));
		return -1;
	}
	// assign address and port to the socket
	struct sockaddr_in server_address;
	memset(&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	server_address.sin_port = htons(PORT);

	// bind socket to specified address
	int bind_result = bind(socket_fd, (struct sockaddr*)&server_address, sizeof(server_address));
	// check if bind was successful
	if(bind_result < 0) {
		fprintf(stderr, "error binding the socket %s\n", strerror(errno));
		return -1;
	}

	return socket_fd;
}

// Receive data from client_socket_fd and send it back to the client.
int echo(int client_socket_fd) {
	char buf[1024];
	int received_size;
	if((received_size = recv(client_socket_fd, &buf, 1024, 0)) < 0) {
		fprintf(stderr, "error receiving data from client: %s\n", strerror(errno));
		return -1;
	}
	send(client_socket_fd, &buf, 1, 0);
	return 0;
}

int main(int argc, char **argv) {
	struct sockaddr_in client;
	int client_fd;
	unsigned int len;

	int server_fd = init_server();
	if(listen(server_fd, 5) == -1) {
		fprintf(stderr, "error listening for connections\n");
		return EXIT_FAILURE;
	}
	printf("Server is listening on port %d for connections...\n", PORT);

	// main server loop
	len = sizeof(client);
	client_fd = accept(server_fd, (struct sockaddr*)&client, &len);
	if(client_fd < 0) {
		fprintf(stderr, "error accepting client connection: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	while(echo(client_fd) >= 0) {}
	close(client_fd);
	

	printf("cleaning up...\n");
	return EXIT_SUCCESS;
}
