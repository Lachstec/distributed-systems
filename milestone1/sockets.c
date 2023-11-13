#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define MAX_STRING_SIZE 128
char buffer[MAX_STRING_SIZE];

void input(int socket_fd) {
    	printf("Enter Text: ");
    	memset(buffer, 0, MAX_STRING_SIZE);
   	fgets(buffer, MAX_STRING_SIZE, stdin);
	write(socket_fd, buffer, sizeof(buffer));
	sleep(1);
}

void output(int socket_fd) {
	read(socket_fd, buffer, sizeof(buffer));
	printf("Text entered: %s\n", buffer);
}

int main(int argc, char **argv) {
	int socket_vec[2];
	if(socketpair(AF_UNIX, SOCK_STREAM, 0, socket_vec) != 0) {
		fprintf(stderr, "error creating socket pair: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	pid_t pid = fork();
	if(pid == 0) {
		printf("input process has pid %d\n", getpid());
		for(;;) {
			input(socket_vec[0]);
		}
	} else {
		printf("output process has pid %d\n", getpid());
		for(;;) {
			output(socket_vec[1]);
		}
	}
}
