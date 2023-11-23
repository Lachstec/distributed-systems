//Max sein Programm

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>

#define MAX_STRING_SIZE 128
char buffer[MAX_STRING_SIZE];
bool input_ready = false;
pthread_mutex_t mutex;

void* input() {
    while(1) {
        if(!input_ready) {
            printf("Enter Text: ");
            memset(buffer, 0, MAX_STRING_SIZE);
            fgets(buffer, MAX_STRING_SIZE, stdin);
            input_ready = true;
        }
    }
    return 0;
}

void output() {
        if (input_ready) {
            printf("Entered Text: %s\n", buffer);
            input_ready = false;
        }
}

int main (int argc, char** argv) {
    pthread_t threadIn;
    //threadOut;


    printf("Communication test (exit with CTRL+c");

    pthread_create(&threadIn, NULL, input, NULL);
    //pthread_create(&threadOut, NULL, output, NULL);
    while(1) {
        output();
    }//pthread_join(threadIn, NULL);
    //pthread_join(threadOut, NULL);



    return 0;
}