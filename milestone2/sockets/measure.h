#ifndef MEASURE_H
#define MEASURE_H
#include <stdint.h>
#include <sys/time.h>
#include <math.h>
#include <stdlib.h>

typedef struct measurement {
	struct timeval start_timestamp;
	struct timeval current_timestamp;
	double_t bandwidth;
	uint64_t total_bytes;
	uint64_t number_measurement_values;
} Measurement;

Measurement* new_measurement();

void get_current_time(Measurement *measurement);

void start_measurement(Measurement *measurement);

void free_measurement(Measurement *measurement);

void measure_bandwidth(Measurement *measurement, uint64_t bytes_read);

#endif