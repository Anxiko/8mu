#include "unity.h"

#include "state.h"
#include "beeper.h"

CpuState actual_cpu_state;

void setUp() {
	init_state(&actual_cpu_state, NULL);
}

void tearDown() {

}

void test_beeper_set_state() {
	bool states[2] = {true, false};
	for (int i = 0; i < 2; ++i) {
		set_beeper_state(&actual_cpu_state, states[i]);
		CpuState expected_state;
		init_state(&expected_state, NULL);
		expected_state.sound_playing = states[i];

		TEST_ASSERT(state_equals(&actual_cpu_state, &expected_state));
	}
}

int main() {
	UNITY_BEGIN();

	RUN_TEST(test_beeper_set_state);

	return UNITY_END();
}