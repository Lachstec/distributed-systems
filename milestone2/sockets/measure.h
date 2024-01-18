#ifndef MEASURE_H
#define MEASURE_H
#include<sys/time.h>

double measure_bandwith(
	struct timeval *start_timestamp,
	struct timeval *current_timestamp,
	float bytes_read
);

#endif