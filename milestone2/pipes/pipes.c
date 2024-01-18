#include <bits/types/struct_timeval.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

#define MAX_STRING_SIZE 128
//2 Buffers to makes sure pipes does not read from the same buffer.
char wbuffer[MAX_STRING_SIZE];
char rbuffer[MAX_STRING_SIZE];

//Process Commmunication with Pipes. Input Process writes while Output Process reads
//This is a unidirectional communication.

void transmitter(int pipeIn[], int pipeOut[]);

void receiver(int pipeIn[], int pipeOut[]);

int main(int argc, char **argv)
{   
    printf("Communication test(exit with CTRL + c)\n");
    int pipefds1[2], pipefds2[2];
    int returnstatus1, returnstatus2;
    int pid;
    //This function creates a pipe [0] is the reading end while [1] is the writing end
    //If something goes wrong during creation, the programm will exit with statuscode (1).
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
        transmitter(pipefds2, pipefds1);
    } else {
        receiver(pipefds1, pipefds2);
    }
    return 0;
}

void transmitter(int pipeIn[], int pipeOut[])
{
    //Reads from stdin
    for(;;){
	long start, end;
	struct timeval timecheck;
	gettimeofday(&timecheck, NULL);
	start = (long)timecheck.tv_sec * 1000 + (long)timecheck.tv_usec /1000;
        printf("Enter Text: ");
        memset(wbuffer, 0, MAX_STRING_SIZE);
        fgets(wbuffer, MAX_STRING_SIZE, stdin);
        //Writes to the pipe
        write(pipeOut[1], wbuffer, sizeof(wbuffer));
        //Sleeps to avoid the "Enter Text" being written to early
	gettimeofday(&timecheck, NULL);
	end = (long)timecheck.tv_sec * 1000 + (long)timecheck.tv_usec /1000; 
	printf("Took %ld microseconds to execute\n", (end-start));
	usleep(100);
    }
}

void receiver(int pipeIn[], int pipeOut[])
{
    for(;;){
        //Reads from the pipe, reading clears the pipe
        read(pipeIn[0],rbuffer,sizeof(rbuffer));
        //prints to stdout
        printf("Entered Text: %s", rbuffer);
    }
}

