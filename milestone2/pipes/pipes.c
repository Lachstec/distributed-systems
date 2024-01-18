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

void transmitter(int pipeRead[], int pipeWrite[]);

void receiver(int pipeRead[], int pipeWrite[]);

ssize_t multi_read(int file, char* buffer, size_t numBytes);

ssize_t multi_write(int file, char* buffer, size_t numBytes);

char* generateData(int KiB){
	//KiB or MiB, not KB and MB
	printf("Generating %dKiB\n", KiB);
	int byte = 1000*KiB;
	char* data = malloc(byte);
	if(data == NULL){
		fprintf(stderr, "failed to allocate %d bytes of memory", byte);
		exit(1);
	}
	memset(data, ' ', byte);
	return data;
}

long getTime();

int main(int argc, char **argv)
{   
	printf("Latency Test (exit with CTRL + c)\n");
	int pipeData[2], pipeACK[2];
	int returnstatus1, returnstatus2;
	int pid;
	//This function creates a pipe [0] is the reading end
	//while [1] is the writing end
	//If something goes wrong during creation, 
	//the programm will exit with statuscode (1).
	returnstatus1=pipe(pipeData);
	returnstatus2=pipe(pipeACK);
	
	if(returnstatus1 == -1 || returnstatus2 == -1){
		fprintf(stderr, "Unable to create pipe\n");
		return 1;
	}
	//Process is forked to create two communicating processes. 
	//The if determines which function should be called.
	pid = fork();
	if(pid==0){
		//pid of 0 means this is the child
        	receiver(pipeData, pipeACK);
    	} else {
		transmitter(pipeACK, pipeData);
    	}
	return 0;
}

void transmitter(int pipeRead[], int pipeWrite[])
{
	//If we want to do the 1KiB, 2KiB, 4KiB ... 8MiB,
	//we can just double initalSize 13 times to get to 8MiB
	int initalSize = 64;
	long totalTime = 0;
    	//Allocate the memory, to send to the other process
	char* memory = generateData(initalSize);
    	for(int i = 0; i < 100; i++){
		long start, end;
		start = getTime();
		//To Check if the full size has been received
		int check;
        	//Writes to the pipe
		if(multi_write(pipeWrite[1], memory, 64000) != (ssize_t)(initalSize*1000))
				fprintf(stderr, "Write error %s()\n",__func__);
		if(read(pipeRead[0], &check, sizeof(check)) != sizeof(check))
			fprintf(stderr, "Read error in %s()\n",__func__);
		end = getTime(); 
		
		totalTime += end - start;
	}
	printf("Total time of %ldms for %dKiB\n", totalTime, initalSize);
	exit(0);
}

void receiver(int pipeRead[], int pipeWrite[])
{	
	int check = 1; 
	int placeholder = 64000;
	char* memory = malloc(placeholder);
	for(int i = 0; i < 100 ; i++){
        	//Reads from the pipe, reading clears the pipe
		if(multi_read(pipeRead[0], memory, placeholder) != placeholder)
				fprintf(stderr, "Read error in %s()\n", __func__);
		if(write(pipeWrite[1], &check, sizeof(check)) != sizeof(check))
			fprintf(stderr, "Write error in %s()\n", __func__);
    	}
}

long getTime(){
	struct timeval timecheck;
	gettimeofday(&timecheck, NULL);
	return (long)timecheck.tv_sec * 1000 + (long)timecheck.tv_usec /1000;
}
//Using ssize_t because read and write can return negative values
ssize_t multi_read(int file, char* buffer, size_t numBytes){
	//printf("Reading %zu Bytes\n", numBytes);
	ssize_t readBytes = 0;
	size_t numLeft = numBytes;
	ssize_t transmitBytes = 0;
	while(numLeft > 0 && (readBytes = read(file, buffer, numLeft)) > 0){
		transmitBytes += readBytes;
		buffer += readBytes;
		numLeft -= readBytes;
	}
	if(transmitBytes == 0)
		transmitBytes = readBytes;
	return transmitBytes;
}

ssize_t multi_write(int file, char* buffer, size_t numBytes){
	//printf("Writing %zu Bytes\n", numBytes);	
	ssize_t writeBytes = 0;
	size_t numLeft = numBytes;
	ssize_t transmitBytes = 0;
	while(numLeft > 0 && (writeBytes = write(file, buffer, numLeft)) > 0){
		transmitBytes += writeBytes;
		buffer += writeBytes;
		numLeft -= writeBytes;
	}
	if(transmitBytes == 0)
		transmitBytes = writeBytes;
	return transmitBytes;
}
