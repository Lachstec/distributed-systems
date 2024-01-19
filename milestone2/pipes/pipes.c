#include <bits/types/struct_timeval.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdlib.h>

//Process Commmunication with Pipes. 'transmitter' sends data to the 'receiver'
//process and 'receiver' sends and 'ACK' when the data is fully received
//This is a bidirectional communication.
//multi_write & multi_read implementation taken from:
//https://stackoverflow.com/a/68879322
//The processes that transmit and receive data
void transmitter(int pipeRead[], int pipeWrite[]);

void receiver(int pipeRead[], int pipeWrite[]);

ssize_t multi_read(int file, char* buffer, size_t numBytes);

ssize_t multi_write(int file, char* buffer, size_t numBytes);

float calc_bandwith(int filesize, int iterations, long time);

char* generateData(int KiB){
	//KiB or MiB, not KB and MB
	printf("Generating %4dKiB\n", KiB);
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
	int returnstatusData, returnstatusACK;
	int pid;
	//This function creates pipes, [0] is the reading end
	//while [1] is the writing end
	//If something goes wrong during creation, 
	//the programm will exit with statuscode (1).
	returnstatusData=pipe(pipeData);
	returnstatusACK=pipe(pipeACK);
	
	if(returnstatusData == -1 || returnstatusACK == -1){
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
	close(pipeData[0]);
	close(pipeData[1]);
	close(pipeACK[0]);
	close(pipeACK[1]);
	exit(0);
}

void transmitter(int pipeRead[], int pipeWrite[]){
	//If we want to do the 1KiB, 2KiB, 4KiB ... 8MiB,
	//we can just double initalSize 13 times to get to 8MiB
	int initalSize = 1;
	long totalTime = 0;
	char* memory = NULL;
    	//Allocate the memory, to send to the other process
	for(int runs = 0; runs < 14; runs++){
		memory = generateData(initalSize);
    		for(int i = 0; i < 100; i++){
			long start, end;
			start = getTime();
			//To Check if the full size has been received
			int check;
        		//Multi-Write to the pipe
			if(multi_write(pipeWrite[1], memory, initalSize*1000) != (ssize_t)(initalSize*1000))
				fprintf(stderr, "Write error %s()\n",__func__);
			//Check if the whole data was received
			if(read(pipeRead[0], &check, sizeof(check)) != sizeof(check))
				fprintf(stderr, "Read error in %s()\n",__func__);
			end = getTime(); 
			totalTime += end - start;
		}
		//Next write tells Receiver that the size is going to change
		//this isnt added to the total time, but is needed for communication
		memory[0] = 'n';
		multi_write(pipeWrite[1], memory, initalSize*1000);
		printf("Total time of %4ldms for %4dKiB, with average of %.4fMB/s\n", totalTime, initalSize, calc_bandwith(initalSize, 100, totalTime));
		initalSize *= 2;
	}
	memory[0] = 'e';
	multi_write(pipeWrite[1], memory, initalSize*1000);
	free(memory);
}

void receiver(int pipeRead[], int pipeWrite[]){	
	int check = 1; 
	int placeholder = 1;
	char* memory = malloc(placeholder*1000);
	for(;;){
		//Reads from the pipe, reading clears the pipe
		if(multi_read(pipeRead[0], memory, placeholder*1000) != placeholder*1000)
			fprintf(stderr, "Read error in %s()\n", __func__);
		if(write(pipeWrite[1], &check, sizeof(check)) != sizeof(check))
			fprintf(stderr, "Write error in %s()\n", __func__);
    		
		if(memory[0] == 'n'){
			placeholder *= 2;
			memory = malloc(placeholder*1000);
		}
		if(memory[0] == 'e'){
			free(memory);
			break;
		}
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

float calc_bandwith(int filesize, int iterations, long time){
	float sentSize = (float)(filesize * iterations)/1024;
	return sentSize/((float)time/1000);
}

