#include "time_millis.h"

int64_t time_millis() {
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);

	return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}