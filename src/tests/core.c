#include "unity.h"

#include "state.h"

CpuState cpu_state;

void setUp(){
	init_state(&cpu_state, NULL);
}

void tearDown() {

}

void test_testing() {
	TEST_ASSERT(1);
}

int main() {
	UNITY_BEGIN();

	RUN_TEST(test_testing);

	return UNITY_END();
}