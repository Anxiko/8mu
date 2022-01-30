#include "instructions.h"

/*
 * Graphics
 */

void clear_screen(uint16_t instruction) {
	memset(get_screen(), 0, SCREEN_SIZE_BYTES);
}

void draw(uint16_t instruction) {
	uint8_t r1 = extract_first_register_from_xyn(instruction);
	uint8_t r2 = extract_second_register_from_xyn(instruction);
	uint8_t x = read_register_bank(r1) % SCREEN_WIDTH;
	uint8_t y = read_register_bank(r2) % SCREEN_HEIGHT;
	uint16_t index_register_value = read_index_register();
	uint8_t n_rows = extract_immediate_from_xyn(instruction);

	uint8_t vf_flag = 0;
	for (uint8_t y_offset = 0; (y_offset < n_rows) && (y + y_offset < SCREEN_HEIGHT); ++y_offset) {
		uint8_t sprite_row = read_byte_memory(index_register_value + y_offset);
		for (uint8_t x_offset = 0; (x_offset < SPRITE_WIDTH) && (x + x_offset < SCREEN_WIDTH); ++x_offset) {
			uint8_t sprite_pixel = (sprite_row >> (7 - x_offset)) & 1;
			uint8_t display_pixel = read_pixel_from_screen(x + x_offset, y + y_offset);

			if (sprite_pixel && display_pixel) {
				vf_flag = 1;
			}

			uint8_t result_pixel = (sprite_pixel ^ display_pixel) & 1;
			write_pixel_to_screen(x + x_offset, y + y_offset, result_pixel);
		}
	}
	write_register_bank(VF_REGISTER, vf_flag);
}


/*
 * Jump/subroutines
 */

void jump(uint16_t instruction) {
	uint16_t destination = instruction & ADDRESS_INSTRUCTION_BITMASK;
	write_register_pc(destination);
}

void jump_subroutine(uint16_t instruction) {
	uint16_t current_address = read_register_pc() & ADDRESS_INSTRUCTION_BITMASK;
	stack_push(current_address);

	uint16_t destination = instruction & ADDRESS_INSTRUCTION_BITMASK;
	write_register_pc(destination);
}

void return_subroutine() {
	uint16_t destination = stack_pop() & ADDRESS_INSTRUCTION_BITMASK;
	write_index_register(destination);
}

/*
 * Conditionals
 */

void skip_if_equal_to_immediate(uint16_t instruction) {
	uint8_t r = extract_register_from_xnn(instruction);
	uint8_t r_value = read_register_bank(r);

	uint8_t immediate = extract_immediate_from_xnn(instruction);
	if (r_value == immediate) {
		write_register_pc(read_register_pc() + INSTRUCTION_SIZE);
	}
}

void skip_if_different_from_immediate(uint16_t instruction) {
	uint8_t r = extract_register_from_xnn(instruction);
	uint8_t r_value = read_register_bank(r);

	uint8_t immediate = extract_immediate_from_xnn(instruction);
	if (r_value != immediate) {
		write_register_pc(read_register_pc() + INSTRUCTION_SIZE);
	}
}

void skip_if_registers_equal(uint16_t instruction) {
	uint8_t r1 = extract_first_register_from_xy(instruction);
	uint8_t r2 = extract_second_register_from_xy(instruction);

	uint8_t r1_val = read_register_bank(r1);
	uint8_t r2_val = read_register_bank(r2);

	if (r1_val == r2_val) {
		write_register_pc(read_register_pc() + INSTRUCTION_SIZE);
	}
}

void skip_if_registers_different(uint16_t instruction) {
	uint8_t r1 = extract_first_register_from_xy(instruction);
	uint8_t r2 = extract_second_register_from_xy(instruction);

	uint8_t r1_val = read_register_bank(r1);
	uint8_t r2_val = read_register_bank(r2);

	if (r1_val != r2_val) {
		write_register_pc(read_register_pc() + INSTRUCTION_SIZE);
	}
}

/*
 * Registers
 */

void set_register_to_immediate(uint16_t instruction) {
	uint8_t r = extract_register_from_xnn(instruction);
	uint8_t immediate = extract_immediate_from_xnn(instruction);

	write_register_bank(r, immediate);
}

void add_immediate_to_register(uint16_t instruction) {
	uint8_t r = extract_register_from_xnn(instruction);
	uint8_t r_val = read_register_bank(r);
	uint8_t immediate = extract_immediate_from_xnn(instruction);

	uint16_t result = r_val + immediate;

	write_register_bank(r, r_val + immediate);
}

void set_index_register(uint16_t instruction) {
	uint16_t i_val = extract_immediate_from_nnn(instruction);
	write_index_register(i_val);
}