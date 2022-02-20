#include "time_millis.h"

int64_t time_millis_return_value;

void mock_set_time_millis(int64_t v) {
	time_millis_return_value = v;
}

int64_t time_millis() {
	return time_millis_return_value;
}