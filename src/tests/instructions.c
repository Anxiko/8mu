#include "unity.h"

#include "state.h"
#include "screen.h"
#include "debug.h"
#include "instructions.h"
#include "mock_time_millis.h"

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
		rv |= ((v >> i) & 1) << (7 - i);
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
	const uint8_t x = SCREEN_WIDTH - 3, y = SCREEN_HEIGHT - 3;

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

void test_skip_if_equal_to_immediate_skip() {
	uint8_t immediate = 0x12;
	uint8_t r = 3; // VX = V3

	uint16_t instruction = 0x3000; // 3XNN
	instruction |= r << INSTRUCTION_FIELD_REGISTER_XNN_OFFSET;
	instruction |= immediate << INSTRUCTION_FIELD_IMMEDIATE_XNN_OFFSET;


	cpu_state.program_counter = 0x200;
	write_register_bank(&cpu_state, r, immediate);

	CpuState expected_cpu_state;
	copy_state(&expected_cpu_state, &cpu_state);
	expected_cpu_state.program_counter = 0x202;

	skip_if_equal_to_immediate(&cpu_state, instruction);

	TEST_ASSERT(state_equals(&expected_cpu_state, &cpu_state));
}

void test_skip_if_equal_to_immediate_no_skip() {
	uint8_t immediate = 0x12;
	uint8_t r = 3; // VX = V3

	uint16_t instruction = 0x3000; // 3XNN
	instruction |= r << INSTRUCTION_FIELD_REGISTER_XNN_OFFSET;
	instruction |= immediate << INSTRUCTION_FIELD_IMMEDIATE_XNN_OFFSET;


	cpu_state.program_counter = 0x200;
	write_register_bank(&cpu_state, r, immediate + 1);

	CpuState expected_cpu_state;
	copy_state(&expected_cpu_state, &cpu_state);

	skip_if_equal_to_immediate(&cpu_state, instruction);

	TEST_ASSERT(state_equals(&expected_cpu_state, &cpu_state));
}

void test_skip_if_different_to_immediate_no_skip() {
	uint8_t immediate = 0x12;
	uint8_t r = 3; // VX = V3

	uint16_t instruction = 0x4000; // 4XNN
	instruction |= r << INSTRUCTION_FIELD_REGISTER_XNN_OFFSET;
	instruction |= immediate << INSTRUCTION_FIELD_IMMEDIATE_XNN_OFFSET;


	cpu_state.program_counter = 0x200;
	write_register_bank(&cpu_state, r, immediate);

	CpuState expected_cpu_state;
	copy_state(&expected_cpu_state, &cpu_state);


	skip_if_different_from_immediate(&cpu_state, instruction);

	TEST_ASSERT(state_equals(&expected_cpu_state, &cpu_state));
}

void test_skip_if_different_to_immediate_skip() {
	uint8_t immediate = 0x12;
	uint8_t r = 3; // VX = V3

	uint16_t instruction = 0x4000; // 4XNN
	instruction |= r << INSTRUCTION_FIELD_REGISTER_XNN_OFFSET;
	instruction |= immediate << INSTRUCTION_FIELD_IMMEDIATE_XNN_OFFSET;


	cpu_state.program_counter = 0x200;
	write_register_bank(&cpu_state, r, immediate + 1);

	CpuState expected_cpu_state;
	copy_state(&expected_cpu_state, &cpu_state);
	expected_cpu_state.program_counter = 0x202;

	skip_if_different_from_immediate(&cpu_state, instruction);

	TEST_ASSERT(state_equals(&expected_cpu_state, &cpu_state));
}

void test_skip_if_registers_equal_skip() {
	uint8_t rx = 3; // VX = V3
	uint8_t ry = 4; // VY = V4
	uint8_t immediate = 0x12;

	uint16_t instruction = 0x5000; // 5XY0
	instruction |= rx << INSTRUCTION_FIELD_FIRST_REGISTER_XY_OFFSET;
	instruction |= ry << INSTRUCTION_FIELD_SECOND_REGISTER_XY_OFFSET;

	write_register_bank(&cpu_state, rx, immediate);
	write_register_bank(&cpu_state, ry, immediate);
	cpu_state.program_counter = 0x200;

	CpuState expected_cpu_state;
	copy_state(&expected_cpu_state, &cpu_state);
	expected_cpu_state.program_counter = 0x202;

	skip_if_registers_equal(&cpu_state, instruction);
	TEST_ASSERT(state_equals(&expected_cpu_state, &cpu_state));
}

void test_skip_if_registers_equal_no_skip() {
	uint8_t rx = 3; // VX = V3
	uint8_t ry = 4; // VY = V4
	uint8_t immediate = 0x12;

	uint16_t instruction = 0x5000; // 5XY0
	instruction |= rx << INSTRUCTION_FIELD_FIRST_REGISTER_XY_OFFSET;
	instruction |= ry << INSTRUCTION_FIELD_SECOND_REGISTER_XY_OFFSET;

	write_register_bank(&cpu_state, rx, immediate);
	write_register_bank(&cpu_state, ry, immediate + 1);
	cpu_state.program_counter = 0x200;

	CpuState expected_cpu_state;
	copy_state(&expected_cpu_state, &cpu_state);

	skip_if_registers_equal(&cpu_state, instruction);
	TEST_ASSERT(state_equals(&expected_cpu_state, &cpu_state));
}

void test_skip_if_registers_different_skip() {
	uint8_t rx = 3; // VX = V3
	uint8_t ry = 4; // VY = V4
	uint8_t immediate = 0x12;

	uint16_t instruction = 0x9000; // 5XY0
	instruction |= rx << INSTRUCTION_FIELD_FIRST_REGISTER_XY_OFFSET;
	instruction |= ry << INSTRUCTION_FIELD_SECOND_REGISTER_XY_OFFSET;

	write_register_bank(&cpu_state, rx, immediate);
	write_register_bank(&cpu_state, ry, immediate + 1);
	cpu_state.program_counter = 0x200;

	CpuState expected_cpu_state;
	copy_state(&expected_cpu_state, &cpu_state);
	expected_cpu_state.program_counter = 0x202;

	skip_if_registers_different(&cpu_state, instruction);
	TEST_ASSERT(state_equals(&expected_cpu_state, &cpu_state));
}

void test_skip_if_registers_different_no_skip() {
	uint8_t rx = 3; // VX = V3
	uint8_t ry = 4; // VY = V4
	uint8_t immediate = 0x12;

	uint16_t instruction = 0x9000; // 5XY0
	instruction |= rx << INSTRUCTION_FIELD_FIRST_REGISTER_XY_OFFSET;
	instruction |= ry << INSTRUCTION_FIELD_SECOND_REGISTER_XY_OFFSET;

	write_register_bank(&cpu_state, rx, immediate);
	write_register_bank(&cpu_state, ry, immediate);
	cpu_state.program_counter = 0x200;

	CpuState expected_cpu_state;
	copy_state(&expected_cpu_state, &cpu_state);

	skip_if_registers_different(&cpu_state, instruction);
	TEST_ASSERT(state_equals(&expected_cpu_state, &cpu_state));
}

void test_skip_if_pressed_skip() {
	uint8_t key = 0xA;
	uint8_t r = 3; // VX = V3

	uint16_t instruction = 0xE09E; // EX9E
	instruction |= r << INSTRUCTION_FIELD_REGISTER_X_OFFSET;

	write_register_bank(&cpu_state, r, key);
	cpu_state.program_counter = 0x200;
	cpu_state.keyboard[key] = true;

	CpuState expected_cpu_state;
	copy_state(&expected_cpu_state, &cpu_state);
	expected_cpu_state.program_counter = 0x202;

	skip_pressed(&cpu_state, instruction);
	TEST_ASSERT(state_equals(&expected_cpu_state, &cpu_state));
}

void test_skip_if_pressed_no_skip() {
	uint8_t key = 0xA;
	uint8_t r = 3; // VX = V3

	uint16_t instruction = 0xE09E; // EX9E
	instruction |= r << INSTRUCTION_FIELD_REGISTER_X_OFFSET;

	write_register_bank(&cpu_state, r, key);
	cpu_state.program_counter = 0x200;
	cpu_state.keyboard[key] = false;

	CpuState expected_cpu_state;
	copy_state(&expected_cpu_state, &cpu_state);

	skip_pressed(&cpu_state, instruction);
	TEST_ASSERT(state_equals(&expected_cpu_state, &cpu_state));
}

void test_copy_register() {
	uint8_t rx = 3;
	uint8_t ry = 4;
	uint16_t rx_val = 0x12;
	uint16_t ry_val = 0x34;

	uint16_t instruction = 0x8000; // 8XY0
	instruction |= rx << INSTRUCTION_FIELD_FIRST_REGISTER_XY_OFFSET;
	instruction |= ry << INSTRUCTION_FIELD_SECOND_REGISTER_XY_OFFSET;

	write_register_bank(&cpu_state, rx, rx_val);
	write_register_bank(&cpu_state, ry, ry_val);

	CpuState expected_cpu_state;
	copy_state(&expected_cpu_state, &cpu_state);
	write_register_bank(&expected_cpu_state, rx, ry_val);

	copy_register(&cpu_state, instruction);
	TEST_ASSERT(state_equals(&expected_cpu_state, &cpu_state));
}

void test_set_register_to_immediate() {
	uint8_t r = 3; // VX = V3
	uint8_t immediate = 0x34;

	uint16_t instruction = 0x6000; // 6XNN
	instruction |= r << INSTRUCTION_FIELD_REGISTER_XNN_OFFSET;
	instruction |= immediate << INSTRUCTION_FIELD_IMMEDIATE_XNN_OFFSET;

	write_register_bank(&cpu_state, r, 0x12);

	CpuState expected_cpu_state;
	copy_state(&expected_cpu_state, &cpu_state);
	write_register_bank(&expected_cpu_state, r, immediate);

	set_register_to_immediate(&cpu_state, instruction);
	TEST_ASSERT(state_equals(&expected_cpu_state, &cpu_state));
}

void test_set_index_register() {
	uint16_t immediate = ADDRESS_BITMASK & 0x123;

	uint16_t instruction = 0xA000; // ANNN
	instruction |= immediate << INSTRUCTION_FIELD_ADDRESS_NNN_OFFSET;

	cpu_state.index_register = 0xABC;

	CpuState expected_cpu_state;
	copy_state(&expected_cpu_state, &cpu_state);
	expected_cpu_state.index_register = immediate;

	set_index_register(&cpu_state, instruction);
	TEST_ASSERT(state_equals(&expected_cpu_state, &cpu_state));
}

void test_dump_one_register_to_memory() {
	uint8_t r = 0x0; // VX = V0
	uint16_t index = 0x200 & ADDRESS_BITMASK; // I = 0x200

	uint16_t instruction = 0xF055; // FX55
	instruction |= r << INSTRUCTION_FIELD_REGISTER_X_OFFSET;

	for (uint8_t i = 0; i < REGISTERS; ++i) {
		write_register_bank(&cpu_state, i, 0xF0 | i);
	}
	write_index_register(&cpu_state, index);

	CpuState expected_cpu_state;
	copy_state(&expected_cpu_state, &cpu_state);
	for (uint8_t i = 0; i <= r; ++i) {
		write_byte_memory(&expected_cpu_state, index + i, 0xF0 | i);
	}
#if OPTION_DUMP_INCREMENTS_I
	write_index_register(&expected_cpu_state, index + r + 1);
#endif

	save_registers(&cpu_state, instruction);
	TEST_ASSERT(state_equals(&expected_cpu_state, &cpu_state));
}

void test_dump_all_registers_to_memory() {
	uint8_t r = 0xF; // VX = VF
	uint16_t index = 0x200 & ADDRESS_BITMASK; // I = 0x200

	uint16_t instruction = 0xF055; // FX55
	instruction |= r << INSTRUCTION_FIELD_REGISTER_X_OFFSET;

	for (uint8_t i = 0; i < REGISTERS; ++i) {
		write_register_bank(&cpu_state, i, 0xF0 | i);
	}
	write_index_register(&cpu_state, index);

	CpuState expected_cpu_state;
	copy_state(&expected_cpu_state, &cpu_state);
	for (uint8_t i = 0; i <= r; ++i) {
		write_byte_memory(&expected_cpu_state, index + i, 0xF0 | i);
	}
#if OPTION_DUMP_INCREMENTS_I
	write_index_register(&expected_cpu_state, index + r + 1);
#endif

	save_registers(&cpu_state, instruction);
	TEST_ASSERT(state_equals(&expected_cpu_state, &cpu_state));
}

void test_load_one_register_from_memory() {
	uint8_t r = 0x0; // VX = V0
	uint16_t index = 0x200 & ADDRESS_BITMASK; // I = 0x200

	uint16_t instruction = 0xF065; // FX65
	instruction |= r << INSTRUCTION_FIELD_REGISTER_X_OFFSET;

	write_index_register(&cpu_state, index);
	for (uint8_t i = 0; i <= REGISTERS; ++i) {
		write_byte_memory(&cpu_state, index + i, 0xF0 | i);
	}

	CpuState expected_cpu_state;
	copy_state(&expected_cpu_state, &cpu_state);
	for (uint8_t i = 0; i <= r; ++i) {
		write_register_bank(&expected_cpu_state, i, 0xF0 | i);
	}

	load_registers(&cpu_state, instruction);
	TEST_ASSERT(state_equals(&expected_cpu_state, &cpu_state));
}

void test_load_all_registers_from_memory() {
	uint8_t r = 0xF; // VX = VF
	uint16_t index = 0x200 & ADDRESS_BITMASK; // I = 0x200

	uint16_t instruction = 0xF065; // FX65
	instruction |= r << INSTRUCTION_FIELD_REGISTER_X_OFFSET;

	write_index_register(&cpu_state, index);
	for (uint8_t i = 0; i <= REGISTERS; ++i) {
		write_byte_memory(&cpu_state, index + i, 0xF0 | i);
	}

	CpuState expected_cpu_state;
	copy_state(&expected_cpu_state, &cpu_state);
	for (uint8_t i = 0; i <= r; ++i) {
		write_register_bank(&expected_cpu_state, i, 0xF0 | i);
	}

	load_registers(&cpu_state, instruction);
	TEST_ASSERT(state_equals(&expected_cpu_state, &cpu_state));
}

void test_add_immediate_to_register() {
	uint8_t r = 0x0; // VX = V0
	uint8_t r0 = 0x0EE; // V0 = 0xF0
	uint8_t immediate = 0x11;

	uint16_t instruction = 0x7000; // 7XNN
	instruction |= r << INSTRUCTION_FIELD_REGISTER_XNN_OFFSET;
	instruction |= immediate << INSTRUCTION_FIELD_IMMEDIATE_XNN_OFFSET;

	write_register_bank(&cpu_state, r, r0);

	CpuState expected_cpu_state;
	copy_state(&expected_cpu_state, &cpu_state);
	write_register_bank(&expected_cpu_state, r, r0 + immediate);

	add_immediate_to_register(&cpu_state, instruction);
	TEST_ASSERT(state_equals(&expected_cpu_state, &cpu_state));
}

void test_add_immediate_to_register_overflow() {
	uint8_t r = 0x0; // VX = V0
	uint8_t r0 = 0xEE; // V0 = 0xF0
	uint8_t immediate = 0x12;

	uint16_t instruction = 0x7000; // 7XNN
	instruction |= r << INSTRUCTION_FIELD_REGISTER_XNN_OFFSET;
	instruction |= immediate << INSTRUCTION_FIELD_IMMEDIATE_XNN_OFFSET;

	write_register_bank(&cpu_state, r, r0);

	CpuState expected_cpu_state;
	copy_state(&expected_cpu_state, &cpu_state);
	write_register_bank(&expected_cpu_state, r, r0 + immediate);

	add_immediate_to_register(&cpu_state, instruction);
	TEST_ASSERT(state_equals(&expected_cpu_state, &cpu_state));
}

void test_add_immediate_to_index() {
	uint8_t r = 0x0; // VX = V0
	uint8_t r0 = 0xEE; // V0 = 0xEE

	uint16_t instruction = 0xF01E; // FX1E
	instruction |= r << INSTRUCTION_FIELD_REGISTER_X_OFFSET;
	write_index_register(&cpu_state, 0xF11);
	write_register_bank(&cpu_state, r, r0);
#if OPTION_OVERFLOW_ON_ADD_TO_INDEX
	write_register_bank(&cpu_state, STATUS_REGISTER, 1);
#endif

	CpuState expected_cpu_state;
	copy_state(&expected_cpu_state, &cpu_state);
	write_index_register(&expected_cpu_state, 0xFFF);
#if OPTION_OVERFLOW_ON_ADD_TO_INDEX
	write_register_bank(&expected_cpu_state, STATUS_REGISTER, 0);
#endif

	add_to_index(&cpu_state, instruction);
	TEST_ASSERT(state_equals(&expected_cpu_state, &cpu_state));
}

void test_add_immediate_to_index_overflow() {
	uint8_t r = 0x0; // VX = V0
	uint8_t r0 = 0xEE; // V0 = 0xEE

	uint16_t instruction = 0xF01E; // FX1E
	instruction |= r << INSTRUCTION_FIELD_REGISTER_X_OFFSET;
	write_index_register(&cpu_state, 0xF12);
	write_register_bank(&cpu_state, r, r0);

	CpuState expected_cpu_state;
	copy_state(&expected_cpu_state, &cpu_state);
	write_index_register(&expected_cpu_state, 0x0);
#if OPTION_OVERFLOW_ON_ADD_TO_INDEX
	write_register_bank(&expected_cpu_state, STATUS_REGISTER, 1);
#endif

	add_to_index(&cpu_state, instruction);
	TEST_ASSERT(state_equals(&expected_cpu_state, &cpu_state));
}

void test_add_register_to_register() {
	uint8_t x = 0x1; // VX = V1
	uint8_t y = 0x2; // VY = V2

	uint8_t xv = 0x12; // VX = 0x12
	uint8_t yv = 0x34; // VY = 0x34

	uint16_t instruction = 0x8004; // 0x8XY4
	instruction |= x << INSTRUCTION_FIELD_FIRST_REGISTER_XY_OFFSET;
	instruction |= y << INSTRUCTION_FIELD_SECOND_REGISTER_XY_OFFSET;

	write_register_bank(&cpu_state, x, xv);
	write_register_bank(&cpu_state, y, yv);
	write_register_bank(&cpu_state, STATUS_REGISTER, 1);

	CpuState expected_cpu_state;
	copy_state(&expected_cpu_state, &cpu_state);
	write_register_bank(&expected_cpu_state, x, 0x46);
	write_register_bank(&expected_cpu_state, STATUS_REGISTER, 0);

	add_register_to_register(&cpu_state, instruction);
	TEST_ASSERT(state_equals(&expected_cpu_state, &cpu_state));
}

void test_add_register_to_register_overflow() {
	uint8_t x = 0x1; // VX = V1
	uint8_t y = 0x2; // VY = V2

	uint8_t xv = 0xEE; // VX = 0xEE
	uint8_t yv = 0x12; // VY = 0x12

	uint16_t instruction = 0x8004; // 0x8XY4
	instruction |= x << INSTRUCTION_FIELD_FIRST_REGISTER_XY_OFFSET;
	instruction |= y << INSTRUCTION_FIELD_SECOND_REGISTER_XY_OFFSET;

	write_register_bank(&cpu_state, x, xv);
	write_register_bank(&cpu_state, y, yv);

	CpuState expected_cpu_state;
	copy_state(&expected_cpu_state, &cpu_state);
	write_register_bank(&expected_cpu_state, x, 0x0);
	write_register_bank(&expected_cpu_state, STATUS_REGISTER, 1);

	add_register_to_register(&cpu_state, instruction);
	TEST_ASSERT(state_equals(&expected_cpu_state, &cpu_state));
}

void test_sub_register_to_register() {
	uint8_t x = 0x1; // VX = V1
	uint8_t y = 0x2; // VY = V2

	uint8_t xv = 0x34; // VX = 0x34
	uint8_t yv = 0x12; // VY = 0x12

	uint16_t instruction = 0x8005; // 0x8XY5
	instruction |= x << INSTRUCTION_FIELD_FIRST_REGISTER_XY_OFFSET;
	instruction |= y << INSTRUCTION_FIELD_SECOND_REGISTER_XY_OFFSET;

	write_register_bank(&cpu_state, x, xv);
	write_register_bank(&cpu_state, y, yv);

	CpuState expected_cpu_state;
	copy_state(&expected_cpu_state, &cpu_state);
	write_register_bank(&expected_cpu_state, x, 0x22);
	write_register_bank(&expected_cpu_state, STATUS_REGISTER, 1);

	sub_register_from_register(&cpu_state, instruction);
	TEST_ASSERT(state_equals(&expected_cpu_state, &cpu_state));
}

void test_sub_register_to_register_underflow() {
	uint8_t x = 0x1; // VX = V1
	uint8_t y = 0x2; // VY = V2

	uint8_t xv = 0x34; // VX = 0x34
	uint8_t yv = 0x35; // VY = 0x35

	uint16_t instruction = 0x8005; // 0x8XY5
	instruction |= x << INSTRUCTION_FIELD_FIRST_REGISTER_XY_OFFSET;
	instruction |= y << INSTRUCTION_FIELD_SECOND_REGISTER_XY_OFFSET;

	write_register_bank(&cpu_state, x, xv);
	write_register_bank(&cpu_state, y, yv);
	write_register_bank(&cpu_state, STATUS_REGISTER, 1);

	CpuState expected_cpu_state;
	copy_state(&expected_cpu_state, &cpu_state);
	write_register_bank(&expected_cpu_state, x, 0xFF);
	write_register_bank(&expected_cpu_state, STATUS_REGISTER, 0);

	sub_register_from_register(&cpu_state, instruction);
	TEST_ASSERT(state_equals(&expected_cpu_state, &cpu_state));
}

void test_negative_sub_register_to_register() {
	uint8_t x = 0x1; // VX = V1
	uint8_t y = 0x2; // VY = V2

	uint8_t xv = 0x12; // VX = 0x12
	uint8_t yv = 0x34; // VY = 0x34

	uint16_t instruction = 0x8005; // 0x8XY5
	instruction |= x << INSTRUCTION_FIELD_FIRST_REGISTER_XY_OFFSET;
	instruction |= y << INSTRUCTION_FIELD_SECOND_REGISTER_XY_OFFSET;

	write_register_bank(&cpu_state, x, xv);
	write_register_bank(&cpu_state, y, yv);

	CpuState expected_cpu_state;
	copy_state(&expected_cpu_state, &cpu_state);
	write_register_bank(&expected_cpu_state, x, 0x22);
	write_register_bank(&expected_cpu_state, STATUS_REGISTER, 1);

	negative_sub_register_from_register(&cpu_state, instruction);
	TEST_ASSERT(state_equals(&expected_cpu_state, &cpu_state));
}

void test_negative_sub_register_to_register_underflow() {
	uint8_t x = 0x1; // VX = V1
	uint8_t y = 0x2; // VY = V2

	uint8_t xv = 0x35; // VX = 0x35
	uint8_t yv = 0x34; // VY = 0x34

	uint16_t instruction = 0x8005; // 0x8XY5
	instruction |= x << INSTRUCTION_FIELD_FIRST_REGISTER_XY_OFFSET;
	instruction |= y << INSTRUCTION_FIELD_SECOND_REGISTER_XY_OFFSET;

	write_register_bank(&cpu_state, x, xv);
	write_register_bank(&cpu_state, y, yv);
	write_register_bank(&cpu_state, STATUS_REGISTER, 1);

	CpuState expected_cpu_state;
	copy_state(&expected_cpu_state, &cpu_state);
	write_register_bank(&expected_cpu_state, x, 0xFF);
	write_register_bank(&expected_cpu_state, STATUS_REGISTER, 0);

	negative_sub_register_from_register(&cpu_state, instruction);
	TEST_ASSERT(state_equals(&expected_cpu_state, &cpu_state));
}

void test_decimal_decode_3_digits() {
	uint8_t x = 0x1; // VX = V1

	uint8_t xv = 123; // VX = 123

	uint16_t instruction = 0xF033; // FX33
	instruction |= x << INSTRUCTION_FIELD_REGISTER_X_OFFSET;

	write_register_bank(&cpu_state, x, xv);
	write_index_register(&cpu_state, 0x200);

	CpuState expected_cpu_state;
	copy_state(&expected_cpu_state, &cpu_state);
	write_byte_memory(&expected_cpu_state, 0x200, 1);
	write_byte_memory(&expected_cpu_state, 0x201, 2);
	write_byte_memory(&expected_cpu_state, 0x202, 3);

	decimal_decode(&cpu_state, instruction);
	TEST_ASSERT(state_equals(&expected_cpu_state, &cpu_state));
}

void test_decimal_decode_2_digits() {
	uint8_t x = 0x1; // VX = V1

	uint8_t xv = 23; // VX = 123

	uint16_t instruction = 0xF033; // FX33
	instruction |= x << INSTRUCTION_FIELD_REGISTER_X_OFFSET;

	write_register_bank(&cpu_state, x, xv);
	write_index_register(&cpu_state, 0x200);

	CpuState expected_cpu_state;
	copy_state(&expected_cpu_state, &cpu_state);
	write_byte_memory(&expected_cpu_state, 0x201, 2);
	write_byte_memory(&expected_cpu_state, 0x202, 3);

	decimal_decode(&cpu_state, instruction);
	TEST_ASSERT(state_equals(&expected_cpu_state, &cpu_state));
}

void test_bitwise_or() {
	uint8_t x = 0x1; //VX = V1
	uint8_t y = 0x2; //VX = V2

	uint8_t xv = 0b10101100;
	uint8_t yv = 0b11001010;

	uint16_t instruction = 0x8001; // 8XY1
	instruction |= x << INSTRUCTION_FIELD_FIRST_REGISTER_XY_OFFSET;
	instruction |= y << INSTRUCTION_FIELD_SECOND_REGISTER_XY_OFFSET;

	write_register_bank(&cpu_state, x, xv);
	write_register_bank(&cpu_state, y, yv);

	CpuState expected_cpu_state;
	copy_state(&expected_cpu_state, &cpu_state);
	write_register_bank(&expected_cpu_state, x, 0b11101110);

	bitwise_or(&cpu_state, instruction);
	TEST_ASSERT(state_equals(&expected_cpu_state, &cpu_state));
}

void test_bitwise_and() {
	uint8_t x = 0x1; //VX = V1
	uint8_t y = 0x2; //VX = V2

	uint8_t xv = 0b10101100;
	uint8_t yv = 0b11001010;

	uint16_t instruction = 0x8002; // 8XY2
	instruction |= x << INSTRUCTION_FIELD_FIRST_REGISTER_XY_OFFSET;
	instruction |= y << INSTRUCTION_FIELD_SECOND_REGISTER_XY_OFFSET;

	write_register_bank(&cpu_state, x, xv);
	write_register_bank(&cpu_state, y, yv);

	CpuState expected_cpu_state;
	copy_state(&expected_cpu_state, &cpu_state);
	write_register_bank(&expected_cpu_state, x, 0b10001000);

	bitwise_and(&cpu_state, instruction);
	TEST_ASSERT(state_equals(&expected_cpu_state, &cpu_state));
}

void test_bitwise_xor() {
	uint8_t x = 0x1; //VX = V1
	uint8_t y = 0x2; //VX = V2

	uint8_t xv = 0b10101100;
	uint8_t yv = 0b11001010;

	uint16_t instruction = 0x8003; // 8XY3
	instruction |= x << INSTRUCTION_FIELD_FIRST_REGISTER_XY_OFFSET;
	instruction |= y << INSTRUCTION_FIELD_SECOND_REGISTER_XY_OFFSET;

	write_register_bank(&cpu_state, x, xv);
	write_register_bank(&cpu_state, y, yv);

	CpuState expected_cpu_state;
	copy_state(&expected_cpu_state, &cpu_state);
	write_register_bank(&expected_cpu_state, x, 0b01100110);

	bitwise_xor(&cpu_state, instruction);
	TEST_ASSERT(state_equals(&expected_cpu_state, &cpu_state));
}

void test_shift_left() {
	uint8_t x = 0x1;
	uint8_t y = 0x2;

	uint8_t v = 0b01010101;
#if OPTION_USE_EXTRA_REGISTER_ON_SHIFT
	write_register_bank(&cpu_state, y, v);
#else
	write_register_bank(&cpu_state, x, v);
#endif
	write_register_bank(&cpu_state, STATUS_REGISTER, 1);

	uint16_t instruction = 0x800E; // 8XYE
	instruction |= x << INSTRUCTION_FIELD_FIRST_REGISTER_XY_OFFSET;
	instruction |= y << INSTRUCTION_FIELD_SECOND_REGISTER_XY_OFFSET;

	CpuState expected_cpu_state;
	copy_state(&expected_cpu_state, &cpu_state);
	write_register_bank(&expected_cpu_state, x, 0b10101010);
	write_register_bank(&expected_cpu_state, STATUS_REGISTER, 0);

	shift_left(&cpu_state, instruction);
	TEST_ASSERT(state_equals(&expected_cpu_state, &cpu_state));
}

void test_shift_left_overflow() {
	uint8_t x = 0x1;
	uint8_t y = 0x2;

	uint8_t v = 0b10101010;
#if OPTION_USE_EXTRA_REGISTER_ON_SHIFT
	write_register_bank(&cpu_state, y, v);
#else
	write_register_bank(&cpu_state, x, v);
#endif

	uint16_t instruction = 0x800E; // 8XYE
	instruction |= x << INSTRUCTION_FIELD_FIRST_REGISTER_XY_OFFSET;
	instruction |= y << INSTRUCTION_FIELD_SECOND_REGISTER_XY_OFFSET;

	CpuState expected_cpu_state;
	copy_state(&expected_cpu_state, &cpu_state);
	write_register_bank(&expected_cpu_state, x, 0b01010100);
	write_register_bank(&expected_cpu_state, STATUS_REGISTER, 1);

	shift_left(&cpu_state, instruction);
	TEST_ASSERT(state_equals(&expected_cpu_state, &cpu_state));
}

void test_shift_right() {
	uint8_t x = 0x1;
	uint8_t y = 0x2;

	uint8_t v = 0b10101010;
#if OPTION_USE_EXTRA_REGISTER_ON_SHIFT
	write_register_bank(&cpu_state, y, v);
#else
	write_register_bank(&cpu_state, x, v);
#endif
	write_register_bank(&cpu_state, STATUS_REGISTER, 1);

	uint16_t instruction = 0x8006; // 8XYE
	instruction |= x << INSTRUCTION_FIELD_FIRST_REGISTER_XY_OFFSET;
	instruction |= y << INSTRUCTION_FIELD_SECOND_REGISTER_XY_OFFSET;

	CpuState expected_cpu_state;
	copy_state(&expected_cpu_state, &cpu_state);
	write_register_bank(&expected_cpu_state, x, 0b01010101);
	write_register_bank(&expected_cpu_state, STATUS_REGISTER, 0);

	shift_right(&cpu_state, instruction);
	TEST_ASSERT(state_equals(&expected_cpu_state, &cpu_state));
}

void test_shift_right_underflow() {
	uint8_t x = 0x1;
	uint8_t y = 0x2;

	uint8_t v = 0b01010101;
#if OPTION_USE_EXTRA_REGISTER_ON_SHIFT
	write_register_bank(&cpu_state, y, v);
#else
	write_register_bank(&cpu_state, x, v);
#endif
	write_register_bank(&cpu_state, STATUS_REGISTER, 1);

	uint16_t instruction = 0x8006; // 8XYE
	instruction |= x << INSTRUCTION_FIELD_FIRST_REGISTER_XY_OFFSET;
	instruction |= y << INSTRUCTION_FIELD_SECOND_REGISTER_XY_OFFSET;

	CpuState expected_cpu_state;
	copy_state(&expected_cpu_state, &cpu_state);
	write_register_bank(&expected_cpu_state, x, 0b00101010);
	write_register_bank(&expected_cpu_state, STATUS_REGISTER, 1);

	shift_right(&cpu_state, instruction);
	TEST_ASSERT(state_equals(&expected_cpu_state, &cpu_state));
}

void test_set_register_to_bitmasked_rand() {
	srand(9943u); // NOLINT(cert-msc51-cpp)
	// Resulting random byte should be 0xFC

	uint8_t x = 0x1;
	uint8_t xv = 0b10101010;
	uint8_t mask = 0xC7;

	uint16_t instruction = 0xC000; // CXNN
	instruction |= x << INSTRUCTION_FIELD_REGISTER_XNN_OFFSET;
	instruction |= mask << INSTRUCTION_FIELD_IMMEDIATE_XNN_OFFSET;

	CpuState expected_cpu_state;
	copy_state(&expected_cpu_state, &cpu_state);
	write_register_bank(&expected_cpu_state, x, 0xC4);

	set_register_to_bitmasked_rand(&cpu_state, instruction);
	TEST_ASSERT(state_equals(&expected_cpu_state, &cpu_state));
}

void test_read_delay() {
	uint8_t x = 0x1;

	uint16_t instruction = 0xF017; // FX07

	instruction |= x << INSTRUCTION_FIELD_REGISTER_X_OFFSET;

	mock_set_time_millis(100); // Time can't be 0 when setting a timer, or it'll be interpreted as not being set
	write_delay_timer(&cpu_state, 50);

	// After half a second, timer should have decremented by 60/2 = 30 ticks, which leaves 20 out of the original 50 remaining
	mock_set_time_millis(600);

	CpuState expected_cpu_state;
	copy_state(&expected_cpu_state, &cpu_state);
	write_register_bank(&expected_cpu_state, x, 20);

	read_delay(&cpu_state, instruction);
	TEST_ASSERT(state_equals(&expected_cpu_state, &cpu_state));
}

void test_set_delay() {
	uint8_t x = 0x1;
	uint8_t xv = 100;

	uint16_t instruction = 0xF015; // FX15
	instruction = x << INSTRUCTION_FIELD_REGISTER_X_OFFSET;

	write_register_bank(&cpu_state, x, xv);

	mock_set_time_millis(100);

	CpuState expected_cpu_state;
	copy_state(&expected_cpu_state, &cpu_state);
	write_delay_timer(&expected_cpu_state, 100);

	set_delay(&cpu_state, instruction);
	TEST_ASSERT(state_equals(&expected_cpu_state, &cpu_state));
}

void test_set_sound() {
	uint8_t x = 0x1;
	uint8_t xv = 100;

	uint16_t instruction = 0xF018; // FX18
	instruction = x << INSTRUCTION_FIELD_REGISTER_X_OFFSET;

	write_register_bank(&cpu_state, x, xv);

	mock_set_time_millis(100);

	CpuState expected_cpu_state;
	copy_state(&expected_cpu_state, &cpu_state);
	write_sound_timer(&expected_cpu_state, 100);

	set_sound(&cpu_state, instruction);
	TEST_ASSERT(state_equals(&expected_cpu_state, &cpu_state));
}

void test_wait_for_key_not_pressed() {
	uint8_t x = 0x1;

	uint16_t instruction = 0xF00A; // FX0A
	instruction |= x << INSTRUCTION_FIELD_REGISTER_X_OFFSET;

	CpuState expected_cpu_state;
	copy_state(&expected_cpu_state, &cpu_state);
	write_register_pc(&expected_cpu_state, ROM_ADDRESS_START - 2);

	wait_for_key(&cpu_state, instruction);
	TEST_ASSERT(state_equals(&expected_cpu_state, &cpu_state));
}

void test_wait_for_key_pressed() {
	uint8_t x = 0x1;
	uint8_t k = 5;

	uint16_t instruction = 0xF00A; // FX0A
	instruction |= x << INSTRUCTION_FIELD_REGISTER_X_OFFSET;

	set_key_pressed(&cpu_state, k, true);

	CpuState expected_cpu_state;
	copy_state(&expected_cpu_state, &cpu_state);
	write_register_bank(&expected_cpu_state, x, k);


	wait_for_key(&cpu_state, instruction);
	TEST_ASSERT(state_equals(&expected_cpu_state, &cpu_state));
}

void test_point_to_char() {
	uint8_t x = 0x1;
	uint8_t character = 7;

	uint16_t instruction = 0xF029; // FX29
	instruction |= x << INSTRUCTION_FIELD_REGISTER_X_OFFSET;

	write_register_bank(&cpu_state, x, character);

	CpuState expected_cpu_state;
	copy_state(&expected_cpu_state, &cpu_state);
	write_index_register(&expected_cpu_state, 0x73);

	point_to_char(&cpu_state, instruction);
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

	RUN_TEST(test_skip_if_equal_to_immediate_skip);
	RUN_TEST(test_skip_if_equal_to_immediate_no_skip);

	RUN_TEST(test_skip_if_different_to_immediate_skip);
	RUN_TEST(test_skip_if_different_to_immediate_no_skip);

	RUN_TEST(test_skip_if_registers_equal_skip);
	RUN_TEST(test_skip_if_registers_equal_no_skip);

	RUN_TEST(test_skip_if_registers_different_skip);
	RUN_TEST(test_skip_if_registers_different_no_skip);

	RUN_TEST(test_skip_if_pressed_skip);
	RUN_TEST(test_skip_if_pressed_no_skip);

	RUN_TEST(test_copy_register);

	RUN_TEST(test_set_register_to_immediate);

	RUN_TEST(test_set_index_register);

	RUN_TEST(test_dump_one_register_to_memory);
	RUN_TEST(test_dump_all_registers_to_memory);

	RUN_TEST(test_load_one_register_from_memory);
	RUN_TEST(test_load_all_registers_from_memory);

	RUN_TEST(test_add_immediate_to_register);
	RUN_TEST(test_add_immediate_to_register_overflow);

	RUN_TEST(test_add_immediate_to_index);
	RUN_TEST(test_add_immediate_to_index_overflow);

	RUN_TEST(test_add_register_to_register);
	RUN_TEST(test_add_register_to_register_overflow);

	RUN_TEST(test_sub_register_to_register);
	RUN_TEST(test_sub_register_to_register_underflow);

	RUN_TEST(test_negative_sub_register_to_register);
	RUN_TEST(test_negative_sub_register_to_register_underflow);

	RUN_TEST(test_decimal_decode_3_digits);
	RUN_TEST(test_decimal_decode_2_digits);

	RUN_TEST(test_bitwise_or);

	RUN_TEST(test_bitwise_and);

	RUN_TEST(test_bitwise_xor);

	RUN_TEST(test_shift_left);
	RUN_TEST(test_shift_left_overflow);

	RUN_TEST(test_shift_right);
	RUN_TEST(test_shift_right_underflow);

	RUN_TEST(test_set_register_to_bitmasked_rand);

	RUN_TEST(test_read_delay);

	RUN_TEST(test_set_delay);

	RUN_TEST(test_set_sound);

	RUN_TEST(test_wait_for_key_not_pressed);
	RUN_TEST(test_wait_for_key_pressed);

	RUN_TEST(test_point_to_char);

	return UNITY_END();
}