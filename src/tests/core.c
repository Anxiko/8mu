#include "unity.h"

#include "state.h"
#include "beeper.h"
#include "keyboard.h"
#include "memory.h"
#include "registers.h"
#include "screen.h"
#include "stack.h"
#include "timers.h"

#include "mock_time_millis.h"

CpuState cpu_state;

void setUp() {
	init_state(&cpu_state, NULL);
	mock_set_time_millis(0);
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

void test_set_pressed() {
	set_key_pressed(&cpu_state, 2, true);
	TEST_ASSERT_TRUE(cpu_state.keyboard[2]);
	set_key_pressed(&cpu_state, 2, false);
	TEST_ASSERT_FALSE(cpu_state.keyboard[2]);
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

void test_register_program_counter() {
	CpuState expected_cpu_state;
	init_state(&expected_cpu_state, NULL);

	write_register_pc(&cpu_state, 0x123);
	expected_cpu_state.program_counter = 0x123;
	TEST_ASSERT(state_equals(&expected_cpu_state, &cpu_state));

	TEST_ASSERT_EQUAL_UINT16(0x123, read_register_pc(&cpu_state));
}

void test_register_index_register() {
	CpuState expected_cpu_state;
	init_state(&expected_cpu_state, NULL);

	write_index_register(&cpu_state, 0x123);
	expected_cpu_state.index_register = 0x123;
	TEST_ASSERT(state_equals(&expected_cpu_state, &cpu_state));

	TEST_ASSERT_EQUAL_UINT16(0x123, read_index_register(&cpu_state));
}

void test_register_bank() {
	uint8_t expected_register_bank[REGISTERS] = {
		0x10, 0x11, 0x12, 0x13,
		0x14, 0x15, 0x16, 0x17,
		0x18, 0x19, 0x1A, 0x1B,
		0x1C, 0x1D, 0x1E, 0x1F
	};
	CpuState expected_cpu_state;
	init_state(&expected_cpu_state, NULL);
	memcpy(expected_cpu_state.register_bank, expected_register_bank, REGISTERS);

	for (uint8_t i = 0; i < REGISTERS; ++i) {
		write_register_bank(&cpu_state, i, i + 0x10);
	}
	TEST_ASSERT(state_equals(&expected_cpu_state, &cpu_state));

	for (uint8_t i = 0; i < REGISTERS; ++i) {
		TEST_ASSERT_EQUAL_UINT8(i + 0x10, read_register_bank(&cpu_state, i));
	}
}

void test_screen_fill_screen() {
	CpuState expected_cpu_state;
	init_state(&expected_cpu_state, NULL);

	bool colors[2] = {COLOR_WHITE, COLOR_BLACK};
	for (int i = 0; i < 2; ++i) {
		fill_screen(&cpu_state, colors[i]);
		memset(expected_cpu_state.display, colors[i] ? 0xFF : 0x00, SCREEN_SIZE_BYTES);
		TEST_ASSERT(state_equals(&expected_cpu_state, &cpu_state));
	}
}

void test_screen_read_pixel_from_screen() {
	const uint8_t display[SCREEN_SIZE_BYTES] = {
		0b10101010,
		// Jump to the next row
		0, 0, 0, 0, 0, 0, 0,
		0b01010101
	};
	memcpy(cpu_state.display, display, SCREEN_SIZE_BYTES);

	TEST_ASSERT_EQUAL_UINT8(COLOR_BLACK, read_pixel_from_screen(&cpu_state, 0, 0));
	TEST_ASSERT_EQUAL_UINT8(COLOR_WHITE, read_pixel_from_screen(&cpu_state, 1, 0));

	TEST_ASSERT_EQUAL_UINT8(COLOR_WHITE, read_pixel_from_screen(&cpu_state, 0, 1));
	TEST_ASSERT_EQUAL_UINT8(COLOR_BLACK, read_pixel_from_screen(&cpu_state, 1, 1));
}

void test_screen_write_pixel_to_screen() {
	CpuState expected_cpu_state;
	init_state(&expected_cpu_state, NULL);
	const uint8_t display[SCREEN_SIZE_BYTES] = {
		0b10101010,
		// Jump to the next row
		0, 0, 0, 0, 0, 0, 0,
		0b01010101
	};
	memcpy(expected_cpu_state.display, display, SCREEN_SIZE_BYTES);

	for (int x = 0; x < 8; ++x) {
		write_pixel_to_screen(&cpu_state, x, 0, x % 2 == 0 ? COLOR_BLACK : COLOR_WHITE);
	}

	for (int x = 0; x < 8; ++x) {
		write_pixel_to_screen(&cpu_state, x, 1, x % 2 == 0 ? COLOR_WHITE : COLOR_BLACK);
	}

	TEST_ASSERT(state_equals(&expected_cpu_state, &cpu_state));
}

void test_stack() {
	CpuState expected_cpu_state;
	init_state(&expected_cpu_state, NULL);
	uint16_t expected_stack[STACK_SIZE] = {
		0x10, 0x11, 0x12, 0x13,
		0x14, 0x15, 0x16, 0x17,
		0x18, 0x19, 0x1A, 0x1B,
		0x1C, 0x1D, 0x1E, 0x1F
	};
	memcpy(expected_cpu_state.stack, expected_stack, 2 * STACK_SIZE);
	expected_cpu_state.stack_size = STACK_SIZE;

	for (int i = 0; i < STACK_SIZE; ++i) {
		stack_push(&cpu_state, i + 0x10);
	}
	TEST_ASSERT(state_equals(&expected_cpu_state, &cpu_state));

	for (int i = STACK_SIZE - 1; i >= 0; --i) {
		TEST_ASSERT_EQUAL_UINT16(i + 0x10, stack_pop(&cpu_state));
	}
	expected_cpu_state.stack_size = 0;
	TEST_ASSERT(state_equals(&expected_cpu_state, &cpu_state));
}

void test_timer_read_delay_timer() {
	CpuState expected_cpu_state;
	init_state(&expected_cpu_state, NULL);

	int64_t time_millis = 1645378224LL * 1000LL; // Real unix epoch timestamp, converted into ms
	mock_set_time_millis(time_millis);

	// Before being set, it should be 0
	TEST_ASSERT_EQUAL_UINT8(0, read_delay_timer(&cpu_state));
	TEST_ASSERT(state_equals(&expected_cpu_state, &cpu_state));

	cpu_state.delay_timer.set_ts_millis = time_millis;
	cpu_state.delay_timer.set_value = 100;
	expected_cpu_state.delay_timer.set_ts_millis = time_millis;
	expected_cpu_state.delay_timer.set_value = 100;

	// After being set, no time has passed
	TEST_ASSERT_EQUAL_UINT8(100, read_delay_timer(&cpu_state));
	TEST_ASSERT(state_equals(&expected_cpu_state, &cpu_state));

	// Pass a second, timer should decrease by 60
	mock_set_time_millis(time_millis + 1000LL);
	TEST_ASSERT_EQUAL_UINT8(40, read_delay_timer(&cpu_state));
	TEST_ASSERT(state_equals(&expected_cpu_state, &cpu_state));

	// Pass the set time of ticks, value should be 0
	mock_set_time_millis(time_millis + 100LL * 1000LL);
	TEST_ASSERT_EQUAL_UINT8(0, read_delay_timer(&cpu_state));
	TEST_ASSERT(state_equals(&expected_cpu_state, &cpu_state));

	// More ticks have passed than the set value
	mock_set_time_millis(time_millis + 101LL * 1000LL);
	TEST_ASSERT_EQUAL_UINT8(0, read_delay_timer(&cpu_state));
	TEST_ASSERT(state_equals(&expected_cpu_state, &cpu_state));

	// Times before the set timer timestamp returns the set value
	mock_set_time_millis(time_millis - 100LL * 1000LL);
	TEST_ASSERT_EQUAL_UINT8(100, read_delay_timer(&cpu_state));
	TEST_ASSERT(state_equals(&expected_cpu_state, &cpu_state));

	// Timer is 0 if timestamp isn't set, even if there is a non-zero value
	cpu_state.delay_timer.set_ts_millis = 0;
	expected_cpu_state.delay_timer.set_ts_millis = 0;
	mock_set_time_millis(time_millis);
	TEST_ASSERT_EQUAL_UINT8(0, read_delay_timer(&cpu_state));
	TEST_ASSERT(state_equals(&expected_cpu_state, &cpu_state));
}

void test_timer_write_delay_timer() {
	CpuState expected_cpu_state;
	init_state(&expected_cpu_state, NULL);

	int64_t time_millis = 1645378224LL * 1000LL; // Real unix epoch timestamp, converted into ms
	mock_set_time_millis(time_millis);

	write_delay_timer(&cpu_state, 100);
	expected_cpu_state.delay_timer.set_ts_millis = time_millis;
	expected_cpu_state.delay_timer.set_value = 100;
	TEST_ASSERT(state_equals(&expected_cpu_state, &cpu_state));
}

void test_timer_write_sound_timer() {
	CpuState expected_cpu_state;
	init_state(&expected_cpu_state, NULL);

	int64_t time_millis = 1645378224LL * 1000LL; // Real unix epoch timestamp, converted into ms
	mock_set_time_millis(time_millis);

	write_sound_timer(&cpu_state, 100);
	expected_cpu_state.sound_timer.set_ts_millis = time_millis;
	expected_cpu_state.sound_timer.set_value = 100;
	TEST_ASSERT(state_equals(&expected_cpu_state, &cpu_state));
}

void test_timer_refresh_timer() {
	CpuState expected_cpu_state;
	init_state(&expected_cpu_state, NULL);

	// Off by default
	update_beeper_status(&expected_cpu_state);
	TEST_ASSERT(state_equals(&expected_cpu_state, &cpu_state));

	int64_t time_millis = 1645378224LL * 1000LL; // Real unix epoch timestamp, converted into ms
	mock_set_time_millis(time_millis);

	cpu_state.sound_timer.set_ts_millis = expected_cpu_state.sound_timer.set_ts_millis = time_millis;
	cpu_state.sound_timer.set_value = expected_cpu_state.sound_timer.set_value = 100;

	// Timer set, no time has passed
	update_beeper_status(&cpu_state);
	expected_cpu_state.sound_playing = true;
	TEST_ASSERT(state_equals(&expected_cpu_state, &cpu_state));

	// Timer set, just expired, sound should have been turned off
	mock_set_time_millis(time_millis + 1000LL * 100LL);
	update_beeper_status(&cpu_state);
	expected_cpu_state.sound_playing = false;
	TEST_ASSERT(state_equals(&expected_cpu_state, &cpu_state));
}

int main() {
	UNITY_BEGIN();

	RUN_TEST(test_beeper_set_beeper_state);

	RUN_TEST(test_set_pressed);
	RUN_TEST(test_keyboard_is_key_pressed);
	RUN_TEST(test_keyboard_any_key_pressed);

	RUN_TEST(test_memory_read);
	RUN_TEST(test_memory_write);
	RUN_TEST(test_memory_character_address);

	RUN_TEST(test_register_program_counter);
	RUN_TEST(test_register_index_register);
	RUN_TEST(test_register_bank);

	RUN_TEST(test_screen_fill_screen);
	RUN_TEST(test_screen_read_pixel_from_screen);
	RUN_TEST(test_screen_write_pixel_to_screen);

	RUN_TEST(test_stack);

	RUN_TEST(test_timer_read_delay_timer);
	RUN_TEST(test_timer_write_delay_timer);
	RUN_TEST(test_timer_write_sound_timer);
	RUN_TEST(test_timer_refresh_timer);

	return UNITY_END();
}