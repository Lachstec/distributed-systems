#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define MAX_STRING_SIZE 128
char wbuffer[MAX_STRING_SIZE];
char rbuffer[MAX_STRING_SIZE];

void input(int array[]);

void output(int array[]);

int main(int argc, char **argv)
{   
    int pipefds[2];
    int returnstatus;
    int pid;
    returnstatus=pipe(pipefds);
    if(returnstatus==-1){
        printf("Unable to create pipe\n");
        return 1;
    }
    pid = fork();
    printf("Communication test(exit with CTRL + c)\n");
    while (1)
    {
        if(pid==0){
            output(pipefds);
        } else {
            input(pipefds);
        }
    }
    return 0;
}

void input(int pipe[])
{
    printf("Enter Text: ");
    memset(wbuffer, 0, MAX_STRING_SIZE);
    fgets(wbuffer, MAX_STRING_SIZE, stdin);
    write(pipe[1], wbuffer, sizeof(wbuffer));
}

void output(int pipe[])
{
    read(pipe[0],rbuffer,sizeof(rbuffer));
    printf("Entered Text: %s\n", rbuffer);
}
