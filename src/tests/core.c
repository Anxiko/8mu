#include "unity.h"

#include "state.h"
#include "beeper.h"
#include "keyboard.h"
#include "memory.h"

CpuState cpu_state;

void setUp() {
	init_state(&cpu_state, NULL);
}

void tearDown() {

}

void test_beeper_set_beeper_state() {
	bool states[2] = {true, false};
	for (int i = 0; i < 2; ++i) {
		set_beeper_state(&cpu_state, states[i]);
		CpuState expected_state;
		init_state(&expected_state, NULL);
		expected_state.sound_playing = states[i];

		TEST_ASSERT(state_equals(&cpu_state, &expected_state));
	}
}

void test_keyboard_is_key_pressed() {
	cpu_state.keyboard[4] = true;
	cpu_state.keyboard[6] = true;

	TEST_ASSERT_TRUE(is_key_pressed(&cpu_state, 4));
	TEST_ASSERT_TRUE(is_key_pressed(&cpu_state, 6));

	cpu_state.keyboard[4] = false;
	TEST_ASSERT_FALSE(is_key_pressed(&cpu_state, 4));
}

void test_keyboard_any_key_pressed() {
	uint8_t pressed_key = 127; // Non-valid key, to check if the value has been altered at all
	TEST_ASSERT_FALSE(any_key_pressed(&cpu_state, &pressed_key));
	TEST_ASSERT_EQUAL_UINT8(127, pressed_key);

	cpu_state.keyboard[4] = true;
	cpu_state.keyboard[6] = true;
	TEST_ASSERT_TRUE(any_key_pressed(&cpu_state, &pressed_key));
	TEST_ASSERT_EQUAL_UINT8(4, pressed_key);
}


void test_memory_read() {
	uint8_t rom[ROM_SIZE] = {0x12, 0x34};
	init_state(&cpu_state, rom);

	TEST_ASSERT_EQUAL_UINT8(0x12, read_byte_memory(&cpu_state, ROM_ADDRESS_START));
	TEST_ASSERT_EQUAL_UINT16(0x1234, read_word_memory(&cpu_state, ROM_ADDRESS_START));
}

void test_memory_write() {
	CpuState expected_state;
	uint8_t rom[ROM_SIZE] = {0x12, 0x34, 0x56};
	init_state(&expected_state, rom);

	write_word_memory(&cpu_state, ROM_ADDRESS_START, 0x1234);
	write_byte_memory(&cpu_state, ROM_ADDRESS_START + 2, 0x56);

	TEST_ASSERT_TRUE(state_equals(&expected_state, &cpu_state));
}

void test_memory_character_address() {
	TEST_ASSERT_EQUAL_UINT16(0x50, character_address(0));
	TEST_ASSERT_EQUAL_UINT16(0x9B, character_address(15));
}

int main() {
	UNITY_BEGIN();

	RUN_TEST(test_beeper_set_beeper_state);

	RUN_TEST(test_keyboard_is_key_pressed);
	RUN_TEST(test_keyboard_any_key_pressed);

	RUN_TEST(test_memory_read);
	RUN_TEST(test_memory_write);
	RUN_TEST(test_memory_character_address);

	return UNITY_END();
}