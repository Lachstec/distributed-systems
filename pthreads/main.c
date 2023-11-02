#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void* render(void* data) {
	for(int i = 0; i < *((int*)data); i += 1) {
		printf("render image\n");
		sleep(1);
	}
	return 0;
}

void* aiCalc(void* data) {
	for(int i = 0; i < *((int*)data); i += 1) {
		printf("calculate enemy pos\n");
		sleep(2);
	}
	return 0;
}

int main(int argc, char** argv) {
	pthread_t thread1, thread2;
	int error;
	int loops = 10;
	
	error = pthread_create(&thread1, NULL, render, &loops);
	if(error != 0) {
		fprintf(stderr, "error creating thread: %d", error);
	}
	error = pthread_create(&thread2, NULL, aiCalc, &loops);
	if(error != 0) {
		fprintf(stderr, "error creating thread: %d", error);
	}

	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);
	return EXIT_SUCCESS;
}
