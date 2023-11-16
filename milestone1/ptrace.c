#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#define MAX_STRING_SIZE 128
char buffer[MAX_STRING_SIZE];

void input()
{
    printf("Enter Text: ");
    memset(buffer, 0, MAX_STRING_SIZE);
    fgets(buffer, MAX_STRING_SIZE, stdin);
    buffer[strcspn(buffer, "\n")] = 0;
    raise(SIGSTOP);
}

void output()
{
    printf("Entered Text: %s\n", buffer);
}

int main(int argc, char **argv) {
	pid_t child = fork();
	if(child == 0) {
		if(ptrace(PTRACE_TRACEME, 0, NULL, NULL) != 0) {
			fprintf(stderr, "error requesting tracing: %s\n", strerror(errno));
			return EXIT_FAILURE;
		}
		for(;;) {
			input();
		}
	} else {
		int status;
		void* addr = (unsigned long int*)buffer;
		long* base = (long*)buffer;
		while(waitpid(child, &status, 0) && ! WIFEXITED(status)) {
			for(int i = 0; i < MAX_STRING_SIZE; i += 1) {
				if(ptrace(PTRACE_PEEKDATA, child, &buffer + (sizeof(long) * i), NULL) != -1) {
					*(base + i) = ptrace(PTRACE_PEEKDATA, child, addr + (sizeof(long) * i), NULL);
					if(*(base + i) == '\0') {
						break;
					}
				} else {
					fprintf(stderr, "error reading memory of tracee: %s\n", strerror(errno));
					return EXIT_FAILURE;
				}
			}
			output();
			ptrace(PTRACE_CONT, child, NULL, NULL);
		}
	}
	
}
