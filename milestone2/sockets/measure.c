#include "measure.h"

// Number of measurments to skip in order to compensate for tcp slow start
#define SKIPS 15

int skips = SKIPS;

Measurement* new_measurement() {
	Measurement *measurement = (Measurement*)malloc(sizeof(Measurement));
	measurement->start_timestamp.tv_sec = 0;
	measurement->start_timestamp.tv_usec = 0;
	measurement->current_timestamp.tv_sec = 0;
	measurement->current_timestamp.tv_usec = 0;
	measurement->number_measurement_values = 0;
	measurement->bandwidth = -1.0;
	measurement->total_bytes = 0;
	return measurement;
}

void get_current_time(Measurement *measurement) {
	gettimeofday(&measurement->current_timestamp, NULL);
}

void start_measurement(Measurement *measurement) {
	gettimeofday(&measurement->start_timestamp, NULL);
}

void free_measurement(Measurement *measurement) {
	free(measurement);
}

double subtract_timeval(struct timeval *lhs, struct timeval *rhs) {
	double difference = lhs->tv_sec - rhs->tv_sec;
	difference += (lhs->tv_usec - rhs->tv_usec) / 1000000.0;
	return difference;
}

void measure_bandwidth(Measurement *measurement, uint64_t bytes_read) {
	measurement->total_bytes += bytes_read;

	if(skips > 0) {
		skips -= 1;
	}

	double ts_diff = subtract_timeval(&measurement->current_timestamp, &measurement->start_timestamp);
	double current_bw = ((double)measurement->total_bytes / (1024 * 1024)) / ts_diff;

	if(measurement->bandwidth < 0) {
		measurement->bandwidth = current_bw;
	} else {
		measurement->bandwidth = (measurement->number_measurement_values + 1) / (( measurement->number_measurement_values / measurement->bandwidth ) + (1 / current_bw ));
	}
	measurement->number_measurement_values += 1;
}