#include "instructions.h"

/* Graphics */

/*
 * 00E0
 * CLEAR
 * Clears the screen.
 */
void clear_screen(uint16_t instruction) {
	memset(get_screen(), 0, SCREEN_SIZE_BYTES);
}


/*
 * DXYN
 * DRAW VX VY N
 * Draws an N rows high pixel at the (VX, VY) coordinates on screen.
 * Set the carry flag if a pixel that was on has been toggled off.
 */
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
	write_register_bank(STATUS_REGISTER, vf_flag);
}


/* Jumping and subroutines */

/*
 * 1NNN
 * GOTO NNN
 * Sets the PC to NNN.
 */
void jump(uint16_t instruction) {
	uint16_t destination = instruction & ADDRESS_INSTRUCTION_BITMASK;
	write_register_pc(destination);
}

/*
 * 2NNN
 * CALL NNN
 * Saves the current PC to the stack, and then sets the PC to NNN.
 */
void jump_subroutine(uint16_t instruction) {
	uint16_t current_address = read_register_pc() & ADDRESS_INSTRUCTION_BITMASK;
	stack_push(current_address);

	uint16_t destination = instruction & ADDRESS_INSTRUCTION_BITMASK;
	write_register_pc(destination);
}

/*
 * BNNN | BXNN
 * JUMPR NNN | JUMPR VX NN
 * Sets the PC to the result of a register + an offset.
 * If OPTION_REGISTER_ARGUMENT_ON_JUMP_WITH_OFFSET, set PC to VX + NN.
 * Otherwise, set PC to V0 + NNN.
 */
void jump_with_offset(uint16_t instruction) {
	uint8_t base;
	uint16_t offset;

#if OPTION_REGISTER_ARGUMENT_ON_JUMP_WITH_OFFSET
	uint8_t vx = extract_register_from_xnn(instruction);
	base = read_register_bank(vx);
	offset = extract_immediate_from_xnn(instruction);
#else
	base = read_register_bank(0);
	offset = extract_register_from_xnn(instruction);
#endif

	uint16_t destination = base + (offset & ADDRESS_INSTRUCTION_BITMASK);
	write_register_pc(destination);
}

/*
 * 00EE
 * RETURN
 * Pops an address from the stack, and sets the PC to this address.
 */
void return_subroutine() {
	uint16_t destination = stack_pop() & ADDRESS_INSTRUCTION_BITMASK;
	write_index_register(destination);
}

/* Conditionals */

/*
 * 3XNN
 * SIEQ VX NN
 * Skips the next instruction if VX is equal to NN.
 */
void skip_if_equal_to_immediate(uint16_t instruction) {
	uint8_t r = extract_register_from_xnn(instruction);
	uint8_t r_value = read_register_bank(r);

	uint8_t immediate = extract_immediate_from_xnn(instruction);
	if (r_value == immediate) {
		write_register_pc(read_register_pc() + INSTRUCTION_SIZE);
	}
}

/*
 * 4XNN
 * SINE VX NN
 * Skips the next instruction if VX is different from NN.
 */
void skip_if_different_from_immediate(uint16_t instruction) {
	uint8_t r = extract_register_from_xnn(instruction);
	uint8_t r_value = read_register_bank(r);

	uint8_t immediate = extract_immediate_from_xnn(instruction);
	if (r_value != immediate) {
		write_register_pc(read_register_pc() + INSTRUCTION_SIZE);
	}
}

/*
 * 5XY0
 * SREQ VX VY
 * Skips the next instruction if VX is equal to VY.
 */
void skip_if_registers_equal(uint16_t instruction) {
	uint8_t r1 = extract_first_register_from_xy(instruction);
	uint8_t r2 = extract_second_register_from_xy(instruction);

	uint8_t r1_val = read_register_bank(r1);
	uint8_t r2_val = read_register_bank(r2);

	if (r1_val == r2_val) {
		write_register_pc(read_register_pc() + INSTRUCTION_SIZE);
	}
}

/*
 * 9XY0
 * SRNE VX VY
 * Skips the next instruction if VX is different from VY.
 */
void skip_if_registers_different(uint16_t instruction) {
	uint8_t r1 = extract_first_register_from_xy(instruction);
	uint8_t r2 = extract_second_register_from_xy(instruction);

	uint8_t r1_val = read_register_bank(r1);
	uint8_t r2_val = read_register_bank(r2);

	if (r1_val != r2_val) {
		write_register_pc(read_register_pc() + INSTRUCTION_SIZE);
	}
}

/* Registers */

/*
 * 6XNN
 * SETR VX NN
 * Sets VX to NN.
 */
void set_register_to_immediate(uint16_t instruction) {
	uint8_t r = extract_register_from_xnn(instruction);
	uint8_t immediate = extract_immediate_from_xnn(instruction);

	write_register_bank(r, immediate);
}

/*
 * ANNN
 * SETI NNN
 * Set the I register to NNN.
 */
void set_index_register(uint16_t instruction) {
	uint16_t i_val = extract_immediate_from_nnn(instruction);
	write_index_register(i_val);
}

/* Arithmetic */

/*
 * 7XNN
 * ADDI VX NN
 * Set VX to VX + NN. Do not set the carry flag on overflow.
 */
void add_immediate_to_register(uint16_t instruction) {
	uint8_t r = extract_register_from_xnn(instruction);
	uint8_t r_val = read_register_bank(r);
	uint8_t immediate = extract_immediate_from_xnn(instruction);

	uint8_t result = r_val + immediate; // Ignore overflow

	write_register_bank(r, result);
}

/*
 * 7XNN
 * ADD VX VY
 * Set VX to VX + VY. Set the carry flag to 1 on overflow, otherwise set it to 0.
 */
void add_register_to_register(uint16_t instruction) {
	uint8_t vx = extract_first_register_from_xy(instruction);
	uint8_t vy = extract_second_register_from_xy(instruction);

	uint8_t vx_val = read_register_bank(vx);
	uint8_t vy_val = read_register_bank(vy);

	// Need something bigger than a byte to check for overflow,
	// result will be trimmed when passed as argument
	uint16_t result = vx_val + vy_val;
	if (result > 255) {
		write_register_bank(STATUS_REGISTER, 1);
	} else {
		write_register_bank(STATUS_REGISTER, 0);
	}

	write_register_bank(vx, result);
}

/*
 * 8XY5
 * SUB VX VY
 * Set VX to VX - VY. Set the carry flag to 1 if VX > VY, otherwise set it to 0.
 */
void sub_register_from_register(uint16_t instruction) {
	uint8_t vx = extract_first_register_from_xy(instruction);
	uint8_t vy = extract_second_register_from_xy(instruction);

	uint8_t vx_val = read_register_bank(vx);
	uint8_t vy_val = read_register_bank(vy);

	if (vx > vy) {
		write_register_bank(STATUS_REGISTER, 1);
	} else {
		write_register_bank(STATUS_REGISTER, 0);
	}

	uint8_t result = vx_val - vy_val;
	write_register_bank(vx, result);
}

/*
 * 8XY7
 * SUB- VX VY
 * Set VX to VY - VX. Set the carry flag to 1 if VY > VX, otherwise set it to 0.
 */
void negative_sub_register_from_register(uint16_t instruction) {
	uint8_t vx = extract_first_register_from_xy(instruction);
	uint8_t vy = extract_second_register_from_xy(instruction);

	uint8_t vx_val = read_register_bank(vx);
	uint8_t vy_val = read_register_bank(vy);

	if (vy > vx) {
		write_register_bank(STATUS_REGISTER, 1);
	} else {
		write_register_bank(STATUS_REGISTER, 0);
	}

	uint8_t result = vy_val - vx_val;
	write_register_bank(vx, result);
}

/* Bit operations */

/*
 * 8XY1
 * OR VX VY
 * Set VX to the bitwise OR of VX and VY.
 */
void bitwise_or(uint16_t instruction) {
	uint8_t vx = extract_first_register_from_xy(instruction);
	uint8_t vy = extract_second_register_from_xy(instruction);

	uint8_t vx_val = read_register_bank(vx);
	uint8_t vy_val = read_register_bank(vy);

	vx_val = vx_val | vy_val;
	write_register_bank(vx, vx_val);
}

/*
 * 8XY2
 * AND VX VY
 * Set VX to the bitwise AND of VX and VY.
 */
void bitwise_and(uint16_t instruction) {
	uint8_t vx = extract_first_register_from_xy(instruction);
	uint8_t vy = extract_second_register_from_xy(instruction);

	uint8_t vx_val = read_register_bank(vx);
	uint8_t vy_val = read_register_bank(vy);

	vx_val = vx_val & vy_val;
	write_register_bank(vx, vx_val);
}

/*
 * 8XY3
 * XOR VX VY
 * Set VX to the bitwise XOR of VX and VY.
 */
void bitwise_xor(uint16_t instruction) {
	uint8_t vx = extract_first_register_from_xy(instruction);
	uint8_t vy = extract_second_register_from_xy(instruction);

	uint8_t vx_val = read_register_bank(vx);
	uint8_t vy_val = read_register_bank(vy);

	vx_val = vx_val ^ vy_val;
	write_register_bank(vx, vx_val);
}

/*
 * 8XYE
 * SHIFTL VX VY.
 * If OPTION_USE_EXTRA_REGISTER_ON_SHIFT, set VX to VY << 1.
 * Otherwise, set VX to VX << 1, ignoring VY entirely.
 * In either case, set the carry flag to the value of the shifted out bit.
 */
void shift_left(uint16_t instruction) {
	uint8_t vx = extract_first_register_from_xy(instruction);
	uint8_t value;

#if OPTION_USE_EXTRA_REGISTER_ON_SHIFT
	uint8_t vy = extract_second_register_from_xy(instruction);
	value = read_register_bank(vy);
#else
	value = read_register_bank(vx);
#endif

	uint8_t shifted_out_bit = (value & 0x80) >> 7;
	write_register_bank(STATUS_REGISTER, shifted_out_bit);

	uint8_t result = value << 1;
	write_register_bank(vx, result);
}

/*
 * 8XY6
 * SHIFTR VX VY.
 * If OPTION_USE_EXTRA_REGISTER_ON_SHIFT, set VX to VY >> 1.
 * Otherwise, set VX to VX >> 1, ignoring VY entirely.
 * In either case, set the carry flag to the value of the shifted out bit.
 */
void shift_right(uint16_t instruction) {
	uint8_t vx = extract_first_register_from_xy(instruction);
	uint8_t value;

#if OPTION_USE_EXTRA_REGISTER_ON_SHIFT
	uint8_t vy = extract_second_register_from_xy(instruction);
	value = read_register_bank(vy);
#else
	value = read_register_bank(vx);
#endif

	uint8_t shifted_out_bit = value & 1;
	write_register_bank(STATUS_REGISTER, shifted_out_bit);

	uint8_t result = value >> 1;
	write_register_bank(vx, result);
}