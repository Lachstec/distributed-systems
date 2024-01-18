#include "measure.h"

// Number of measurments to skip in order to compensate for slow start
#define SKIPS 15

double bandwith = -1;
float total_bytes = 0;
int number_measurement_values = 0;
int skips = SKIPS;

double subtract_timeval(struct timeval *lhs, struct timeval *rhs) {
	double difference = lhs->tv_sec - rhs->tv_sec;
	difference += (lhs->tv_usec - rhs->tv_usec) / 1000000.0;
	return difference;
}

double measure_bandwith(
	struct timeval *start_timestamp,
	struct timeval *current_timestamp,
	float bytes_read
	)
{
	total_bytes += bytes_read;

	if(skips > 0) {
		skips -= 1;
		return bandwith;
	}

	double ts_diff = subtract_timeval(current_timestamp, start_timestamp);
	double current_bw = (total_bytes / (1024 * 1024)) / ts_diff;

	// 
	if(bandwith < 0) {
		bandwith = current_bw;
	} else {
		bandwith = (number_measurement_values + 1) / (( number_measurement_values / bandwith ) + (1 / current_bw ));
	}
	number_measurement_values += 1;
	return bandwith;
}