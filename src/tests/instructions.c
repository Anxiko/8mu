#include "unity.h"

#include "state.h"
#include "screen.h"
#include "debug.h"
#include "instructions.h"

CpuState cpu_state;

void setUp() {
	init_state(&cpu_state, NULL);
}

void tearDown() {

}

// Helpers

uint8_t reorder_byte_bits(uint8_t v) {
	uint8_t rv = 0;
	for (int i = 0; i < 8; ++i) {
		rv |= ((v >> i) & 1) << (7-i);
	}
	return rv;
}


// Draws some raws to a screen, but without XORing
void draw_to_expected_cpu_state(
	CpuState *expected_cpu_state, uint8_t x, uint8_t y, const uint8_t rows[], uint8_t n_rows, bool draw_second_byte
) {
	int pixel_address = y * SCREEN_WIDTH + x;
	int offset = pixel_address % 8;
	int byte_address = pixel_address / 8;

	for (int i = 0; i < n_rows; ++i) {
		uint16_t row = rows[i] << (8 - offset);
		uint8_t first_byte = (row & 0xFF00) >> 8;
		expected_cpu_state->display[byte_address] = reorder_byte_bits(first_byte);
		if (offset > 0 && draw_second_byte) {
			uint8_t second_byte = row & 0x00FF;
			expected_cpu_state->display[byte_address + 1] = reorder_byte_bits(second_byte);
		}
		byte_address += SCREEN_WIDTH / 8;
	}
}

void test_clear_screen() {
	cpu_state.display[0] = 0xFF; // Set a pixel row to white, it should be cleared out

	CpuState expected_cpu_state;
	init_state(&expected_cpu_state, NULL);

	uint16_t instruction = 0x0; // The instruction should not matter for this instruction clear_screen at all

	clear_screen(&cpu_state, instruction);

	TEST_ASSERT(state_equals(&expected_cpu_state, &cpu_state));
}

// Draw is a complicated instruction, so we'll have a few tests for it

// Draw onto a blank screen, no collision with other sprites
void test_draw_blank() {
	const uint8_t x = 30, y = 20;

	uint16_t character_zero = character_address(0);
	cpu_state.index_register = character_zero;
	cpu_state.register_bank[0] = x;
	cpu_state.register_bank[1] = y;

	CpuState expected_cpu_state;
	copy_state(&expected_cpu_state, &cpu_state);

	uint16_t instruction = 0xD000;
	instruction |= 0x0 << 8; // VX = V0
	instruction |= 0x1 << 4; // VY = V1
	instruction |= 0x5 << 0; // N = 5

	const uint8_t rows[5] = {0xF0, 0x90, 0x90, 0x90, 0xF0};
	draw_to_expected_cpu_state(
		&expected_cpu_state, x, y, rows, sizeof(rows), true);

	draw(&cpu_state, instruction);

	TEST_ASSERT(state_equals(&expected_cpu_state, &cpu_state));
}

// Draw on top of some white pixels, so they cause a collision
void test_draw_on_top() {
	const uint8_t x = 30, y = 20;

	uint16_t character_zero = character_address(0);
	cpu_state.index_register = character_zero;
	cpu_state.register_bank[0] = x;
	cpu_state.register_bank[1] = y;

	write_pixel_to_screen(&cpu_state, x, y, COLOR_WHITE);

	CpuState expected_cpu_state;
	copy_state(&expected_cpu_state, &cpu_state);

	uint16_t instruction = 0xD000;
	instruction |= 0x0 << 8; // VX = V0
	instruction |= 0x1 << 4; // VY = V1
	instruction |= 0x5 << 0; // N = 5

	const uint8_t rows[5] = {0xF0 ^ 0x80, 0x90, 0x90, 0x90, 0xF0};
	draw_to_expected_cpu_state(
		&expected_cpu_state, x, y, rows, sizeof(rows), true);
	expected_cpu_state.register_bank[STATUS_REGISTER] = 0x01;

	draw(&cpu_state, instruction);

	TEST_ASSERT(state_equals(&expected_cpu_state, &cpu_state));
}

// Draw outside the screen, drawings shouldn't warp
void test_draw_no_wrap() {
	const uint8_t x = SCREEN_WIDTH - 3 , y = SCREEN_HEIGHT - 3;

	uint16_t character_zero = character_address(0);
	cpu_state.index_register = character_zero;
	cpu_state.register_bank[0] = x;
	cpu_state.register_bank[1] = y;

	CpuState expected_cpu_state;
	copy_state(&expected_cpu_state, &cpu_state);

	uint16_t instruction = 0xD000;
	instruction |= 0x0 << 8; // VX = V0
	instruction |= 0x1 << 4; // VY = V1
	instruction |= 0x5 << 0; // N = 5

	const uint8_t rows[3] = {0xE0, 0x80, 0x80};
	draw_to_expected_cpu_state(
		&expected_cpu_state, x, y, rows, sizeof(rows), false);

	draw(&cpu_state, instruction);

	TEST_ASSERT(state_equals(&expected_cpu_state, &cpu_state));
}

void test_jump() {
	uint16_t instruction = 0x1000;
	instruction |= 0xABC; // Address

	CpuState expected_cpu_state;
	copy_state(&expected_cpu_state, &cpu_state);
	expected_cpu_state.program_counter = 0xABC;

	jump(&cpu_state, instruction);

	TEST_ASSERT(state_equals(&expected_cpu_state, &cpu_state));
}

void test_jump_subroutine() {
	cpu_state.program_counter = 0xABC;
	CpuState expected_cpu_state;
	copy_state(&expected_cpu_state, &cpu_state);
	expected_cpu_state.program_counter = 0xDEF;
	expected_cpu_state.stack_size = 1;
	expected_cpu_state.stack[0] = 0xABC;

	uint16_t instruction = 0x2000;
	instruction |= 0xDEF; // Address

	jump_subroutine(&cpu_state, instruction);

	TEST_ASSERT(state_equals(&expected_cpu_state, &cpu_state));
}

void test_jump_with_offset() {
	uint16_t instruction = 0xB000;
	uint16_t expected_pc = 0x45;

#if OPTION_REGISTER_ARGUMENT_ON_JUMP_WITH_OFFSET
	instruction |= 0x5 << 8; // VX = V5
	instruction |= 0x23; // Offset
	cpu_state.register_bank[5] = 0x45;

	expected_pc += 0x23;
#else
	instruction |= 0x123; // Offset
	cpu_state.register_bank[0] = 0x45;

	expected_pc += 0x123;
#endif
	CpuState expected_cpu_state;
	copy_state(&expected_cpu_state, &cpu_state);
	expected_cpu_state.program_counter = expected_pc;

	jump_with_offset(&cpu_state, instruction);
	TEST_ASSERT(state_equals(&expected_cpu_state, &cpu_state));
}

void test_return_subroutine() {
	cpu_state.program_counter = ADDRESS_BITMASK & 0x0BCD;
	stack_push(&cpu_state, ADDRESS_BITMASK & 0x0ABC);
	CpuState expected_cpu_state;
	copy_state(&expected_cpu_state, &cpu_state);
	expected_cpu_state.stack_size = 0;
	expected_cpu_state.program_counter = 0xABC;

	return_subroutine(&cpu_state, 0x00EE);
	TEST_ASSERT(state_equals(&expected_cpu_state, &cpu_state));
}

int main() {
	UNITY_BEGIN();

	RUN_TEST(test_clear_screen);

	RUN_TEST(test_draw_blank);
	RUN_TEST(test_draw_on_top);
	RUN_TEST(test_draw_no_wrap);

	RUN_TEST(test_jump);

	RUN_TEST(test_jump_subroutine);

	RUN_TEST(test_jump_with_offset);

	RUN_TEST(test_return_subroutine);

	return UNITY_END();
}