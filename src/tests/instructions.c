#include "unity.h"

#include "state.h"
#include "screen.h"
#include "instructions.h"

CpuState cpu_state;

void setUp() {
	init_state(&cpu_state, NULL);
}

void tearDown() {

}

void test_clear_screen() {
	cpu_state.display[0] = 0xFF; // Set a pixel to white, it should be cleared out

	CpuState expected_cpu_state;
	init_state(&expected_cpu_state, NULL);

	int16_t instruction = 0x0; // The instruction should not matter for this instruction clear_screen at all

	clear_screen(&cpu_state, instruction);

	TEST_ASSERT(state_equals(&expected_cpu_state, &cpu_state));
}


int main() {
	UNITY_BEGIN();

	RUN_TEST(test_clear_screen);

	return UNITY_END();
}