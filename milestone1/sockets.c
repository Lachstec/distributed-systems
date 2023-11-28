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

// input takes a socket file descriptor and uses it to read in a string from the command line
// and write it to the socket. It then sleeps for 100ms.
void input(int socket_fd) {
	for(;;) {

		printf("Enter Text: (quit to exit)");
		memset(buffer, 0, MAX_STRING_SIZE);
		fgets(buffer, MAX_STRING_SIZE, stdin);
		// remove the pesky \n!
		buffer[strcspn(buffer, "\n")] = 0;
		if(strcmp(buffer, "quit") == 0) {
			write(socket_fd, "quit", 4);
			break;
		}
		write(socket_fd, buffer, sizeof(buffer));
		// this is just to wait for the output process, the actual message gets discarded
		read(socket_fd, buffer, sizeof(buffer));
	}
	// close the socket when done
	close(socket_fd);
}

// output takes a socket file descriptor and reads from it to print it to the console.
void output(int socket_fd) {
	for(;;) {
		memset(buffer, 0, MAX_STRING_SIZE);
		read(socket_fd, buffer, sizeof(buffer));
		if(strcmp(buffer, "quit") == 0) {
			printf("Bye!\n");
			break;
		}
		printf("Text entered: %s\n", buffer);
		write(socket_fd, "a", 1);
	}
	// close the socket when done
	close(socket_fd);
}

int main(int argc, char **argv) {
	// array to store the socket file descriptors
	int socket_vec[2];
	// use the socketpair to create two sockets and store them into socket_vec.
	// if an error occurs, it gets printed and the program returns EXIT_FAILURE.
	if(socketpair(AF_UNIX, SOCK_STREAM, 0, socket_vec) != 0) {
		fprintf(stderr, "error creating socket pair: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	// fork and use on process for input and the other for output.
	pid_t pid = fork();
	if(pid == 0) {
		printf("input process has pid %d\n", getpid());
		input(socket_vec[0]);	
	} else {
		printf("output process has pid %d\n", getpid());
		output(socket_vec[1]);	
	}
	return EXIT_SUCCESS;
}
