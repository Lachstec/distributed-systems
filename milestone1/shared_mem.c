#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
// identifies the shared memory allocation, not a pointer
#define SHARED_MEM_IDENT 0x1234
#define BUFFER_LEN 128
char buffer[BUFFER_LEN];

int main(int argc, char** argv) {
	// the unique identifier to refer to the shared memory segment
	int shm_id;
	// pointer to the shared memory buffer
	char* shm_buffer;
	// allocate a shared memory buffer with BUFFER_LEN size and with read/write permissions
	if((shm_id = shmget(SHARED_MEM_IDENT, BUFFER_LEN, IPC_CREAT | SHM_R | SHM_W)) < 0) {
		perror("error creating shared memory");
		return EXIT_FAILURE;
	}
	// obtain a pointer to the previously requested shared memory area
	if((shm_buffer = (char*)shmat(shm_id, NULL, 0)) < 0) {
		perror("error attaching shared memory");
		return EXIT_FAILURE;
	}
	// you should fork here and perform the logic as in the example in base.c
	*shm_buffer = 'C';
	printf("%c\n", *shm_buffer);
	// detach the shared memory buffer 
	shmdt(shm_buffer);
	// mark shared memory segment for destruction to free resources
	shmctl(SHARED_MEM_IDENT, IPC_RMID, NULL);
	return EXIT_SUCCESS;
}
