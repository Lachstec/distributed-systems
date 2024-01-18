#include <bits/types/struct_timeval.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdlib.h>

#define MAX_STRING_SIZE 128
//2 Buffers to makes sure pipes does not read from the same buffer.
char wbuffer[MAX_STRING_SIZE];
char rbuffer[MAX_STRING_SIZE];

//Process Commmunication with Pipes. Input Process writes while Output Process reads
//This is a unidirectional communication.

void transmitter(int pipeIn[], int pipeOut[], int value);

void receiver(int pipeIn[], int pipeOut[]);

long getTime();

int main(int argc, char **argv)
{   
	printf("Latency Test (exit with CTRL + c)\n");
	int pipefds1[2], pipefds2[2];
	int returnstatus1, returnstatus2;
	int pid;
	//This function creates a pipe [0] is the reading end
	//while [1] is the writing end
	//If something goes wrong during creation, 
	//the programm will exit with statuscode (1).
	returnstatus1=pipe(pipefds1);
	returnstatus2=pipe(pipefds2);
	
	if(returnstatus1 == -1 || returnstatus2 == -1){
		fprintf(stderr, "Unable to create pipe\n");
		return 1;
	}
	//Process is forked to create two communicating processes. 
	//The if determines which function should be called.
	pid = fork();
	if(pid==0){
		//pid of 0 means this is the child
        	int value = 1;
		transmitter(pipefds2, pipefds1, value);
    	} else {
        	receiver(pipefds1, pipefds2);
    	}
	return 0;
}

void transmitter(int pipeIn[], int pipeOut[], int value)
{
	//If we want to do the 1KiB, 2KiB, 4KiB ... 8MiB,
	//we can just double initalSize 13 times to get to 8MiB
	int initalSize = 1;
	long totalTime = 0;
    	//Allocate the memory, to send to the other process
	int* memory = malloc(initalSize*1000);
    	for(int i = 0; i < 100; i++){
		long start, end;
		start = getTime();
        	//Writes to the pipe
		if(value == 1){
			write(pipeOut[1], &value, sizeof(value));
			value = 0;
		}
		read(pipeIn[0], &value, sizeof(value));
		if(value == 1){
			end = getTime(); 
			//printf("Took %ld milliseconds to execute\n", (end-start));	
		}
		totalTime += end - start;
	}
	printf("Total time of %ldms for %dKiB\n", totalTime, initalSize);
	exit(0);
}

void receiver(int pipeIn[], int pipeOut[])
{
	for(;;){
        	//Reads from the pipe, reading clears the pipe
        	int value = 0;
		read(pipeIn[0], &value,sizeof(value));
		if(value == 1){
			//printf("Recieved Data\n");
			write(pipeOut[1], &value, sizeof(value));
			value = 0;
		}
    	}
}

long getTime(){
	struct timeval timecheck;
	gettimeofday(&timecheck, NULL);
	return (long)timecheck.tv_sec * 1000 + (long)timecheck.tv_usec /1000;
}
