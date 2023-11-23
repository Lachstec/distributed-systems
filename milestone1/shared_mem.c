#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <string.h>
#include <sys/shm.h>
#include <unistd.h>
#include <errno.h>
// identifies the shared memory allocation, not a pointer
#define SHARED_MEM_IDENT 0x1234
#define BUFFER_LEN 128
char buffer[BUFFER_LEN];
char* shm_buffer;
char* shm_buffer2;
#define SHARED_MEM_IDENT2 0x2345

void input() {
        printf("Enter Text: ");
        memset(buffer, 0, BUFFER_LEN);
        fgets(shm_buffer, BUFFER_LEN, stdin);
        *shm_buffer2='b';
}

void output(){
        printf("Entered Text: %s\n",shm_buffer);
        *shm_buffer2='a';
}

int main(int argc, char** argv) {
        int pid;
        // the unique identifier to refer to the shared memory segment
        int shm_id;
        int shm_id2;
        // pointer to the shared memory buffer

        // allocate a shared memory buffer with BUFFER_LEN size and with read/write permissions
        if((shm_id = shmget(SHARED_MEM_IDENT, BUFFER_LEN, IPC_CREAT | SHM_R | SHM_W)) < 0) {
                perror("error creating shared memory");
                return EXIT_FAILURE;
        }
        if((shm_id2=shmget(SHARED_MEM_IDENT2, BUFFER_LEN, IPC_CREAT | SHM_R | SHM_W)) < 0) {
                perror("error creating shared memory");
                return EXIT_FAILURE;
        }
        // obtain a pointer to the previously requested shared memory area
        if((shm_buffer = (char*)shmat(shm_id, NULL, 0)) < 0) {
                perror("error attaching shared memory");
                return EXIT_FAILURE;
        }
        if((shm_buffer2 = (char*)shmat(shm_id2, NULL, 0)) < 0) {
                perror("error attaching shared memory");
                return EXIT_FAILURE;
        }
        pid = fork();
        *shm_buffer2 = 'a';
        while(1){
        if(pid == 0){//Child process
                if(*shm_buffer2=='a'){
                        input();
                }
        } else { //Parent process
                if(*shm_buffer2=='b'){
                        output();
                }

        }}

        /*
        *shm_buffer = 'C';
        printf("%c\n", *shm_buffer);
        // detach the shared memory buffer
        shmdt(shm_buffer);
        // mark shared memory segment for destruction to free resources
        shmctl(SHARED_MEM_IDENT, IPC_RMID, NULL);
        */
        return EXIT_SUCCESS;
}