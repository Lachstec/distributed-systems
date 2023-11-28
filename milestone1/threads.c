//Max sein Programm

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>

#define MAX_STRING_SIZE 128
char buffer[MAX_STRING_SIZE];
bool input_ready = false;

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
    printf("Communication test (exit with CTRL+c");

    pthread_create(&threadIn, NULL, input, NULL);
    while(1) {
        output();
    }
    return 0;
}
