#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define MAX_STRING_SIZE 128
//2 Buffers to makes sure pipes does not read from the same buffer.
char wbuffer[MAX_STRING_SIZE];
char rbuffer[MAX_STRING_SIZE];

//Process Commmunication with Pipes. Input Process writes while Output Process reads
//This is a unidirectional communication.

void input(int array[]);

void output(int array[]);

int main(int argc, char **argv)
{   
    printf("Communication test(exit with CTRL + c)\n");
    int pipefds[2];
    int returnstatus;
    int pid;
    //This function creates a pipe [0] is the reading end while [1] is the writing end
    //If something goes wrong during creation, the programm will exit with statuscode (1).
    returnstatus=pipe(pipefds);
    if(returnstatus==-1){
        fprintf(stderr, "Unable to create pipe\n");
        return 1;
    }
    //Process is forked to create two communicating processes. 
    //The if determines which function should be called.
    pid = fork();
    //while (1)
    //{
        if(pid==0){
            //pid of 0 means this is the child
            output(pipefds);
        } else {
            input(pipefds);
        }
    //}
    return 0;
}

void input(int pipe[])
{
    //Reads from stdin
    for(;;){
    printf("Enter Text: ");
    memset(wbuffer, 0, MAX_STRING_SIZE);
    fgets(wbuffer, MAX_STRING_SIZE, stdin);
    //Writes to the pipe
    write(pipe[1], wbuffer, sizeof(wbuffer));
    //Sleeps to avoid the "Enter Text" being written to early
    usleep(100);
    }
}

void output(int pipe[])
{
    for(;;){
    //Reads from the pipe, reading clears the pipe
    read(pipe[0],rbuffer,sizeof(rbuffer));
    //prints to stdout
    printf("Entered Text: %s", rbuffer);
    }
}
