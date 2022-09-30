#include "instructions.h"

/* Graphics */

/*
 * 00E0
 * CLEAR
 * Clears the screen.
 */
void clear_screen(CpuState *cpu_state, __attribute__((unused)) uint16_t _instruction) {
	fill_screen(cpu_state, COLOR_BLACK);
}


/*
 * DXYN
 * DRAW VX VY N
 * Draws an N rows high pixel at the (VX, VY) coordinates on screen.
 * Set the carry flag if a pixel that was on has been toggled off.
 */
void draw(CpuState *cpu_state, uint16_t instruction) {
	uint8_t r1 = extract_first_register_from_xyn(instruction);
	uint8_t r2 = extract_second_register_from_xyn(instruction);
	uint8_t x = read_register_bank(cpu_state, r1) % SCREEN_WIDTH;
	uint8_t y = read_register_bank(cpu_state, r2) % SCREEN_HEIGHT;
	uint16_t index_register_value = read_index_register(cpu_state);
	uint8_t n_rows = extract_immediate_from_xyn(instruction);

	uint8_t vf_flag = 0;
	for (uint8_t y_offset = 0; (y_offset < n_rows) && (y + y_offset < SCREEN_HEIGHT); ++y_offset) {
		uint8_t sprite_row = read_byte_memory(cpu_state, index_register_value + y_offset);
		for (uint8_t x_offset = 0; (x_offset < SPRITE_WIDTH) && (x + x_offset < SCREEN_WIDTH); ++x_offset) {
			uint8_t sprite_pixel = (sprite_row >> (7 - x_offset)) & 1;
			uint8_t display_pixel = read_pixel_from_screen(cpu_state, x + x_offset, y + y_offset);

			if (sprite_pixel && display_pixel) {
				vf_flag = 1;
			}

			uint8_t result_pixel = (sprite_pixel ^ display_pixel) & 1;
			write_pixel_to_screen(cpu_state, x + x_offset, y + y_offset, result_pixel);
		}
	}
	write_register_bank(cpu_state, STATUS_REGISTER, vf_flag);
}


/* Jumping and subroutines */

/*
 * 1NNN
 * GOTO NNN
 * Sets the PC to NNN.
 */
void jump(CpuState *cpu_state, uint16_t instruction) {
	uint16_t destination = instruction & ADDRESS_BITMASK;
	write_register_pc(cpu_state, destination);
}

/*
 * 2NNN
 * CALL NNN
 * Saves the current PC to the stack, and then sets the PC to NNN.
 */
void jump_subroutine(CpuState *cpu_state, uint16_t instruction) {
	uint16_t current_address = read_register_pc(cpu_state) & ADDRESS_BITMASK;
	stack_push(cpu_state, current_address);

	uint16_t destination = instruction & ADDRESS_BITMASK;
	write_register_pc(cpu_state, destination);
}

/*
 * BNNN | BXNN
 * JUMPR NNN | JUMPR VX NN
 * Sets the PC to the result of a register + an offset.
 * If OPTION_REGISTER_ARGUMENT_ON_JUMP_WITH_OFFSET, set PC to VX + NN.
 * Otherwise, set PC to V0 + NNN.
 */
void jump_with_offset(CpuState *cpu_state, uint16_t instruction) {
	uint8_t base;
	uint16_t offset;

#if OPTION_REGISTER_ARGUMENT_ON_JUMP_WITH_OFFSET
	uint8_t vx = extract_register_from_xnn(instruction);
	base = read_register_bank(cpu_state, vx);
	offset = extract_immediate_from_xnn(instruction);
#else
	base = read_register_bank(cpu_state, 0);
	offset = extract_immediate_from_nnn(instruction);
#endif

	uint16_t destination = base + (offset & ADDRESS_BITMASK);
	write_register_pc(cpu_state, destination);
}

/*
 * 00EE
 * RETURN
 * Pops an address from the stack, and sets the PC to this address.
 */
void return_subroutine(CpuState *cpu_state, __attribute__((unused)) uint16_t _instruction) {
	uint16_t destination = stack_pop(cpu_state) & ADDRESS_BITMASK;
	write_register_pc(cpu_state, destination);
}

/* Conditionals */

/*
 * 3XNN
 * SIEQ VX NN
 * Skips the next instruction if VX is equal to NN.
 */
void skip_if_equal_to_immediate(CpuState *cpu_state, uint16_t instruction) {
	uint8_t r = extract_register_from_xnn(instruction);
	uint8_t r_value = read_register_bank(cpu_state, r);

	uint8_t immediate = extract_immediate_from_xnn(instruction);
	if (r_value == immediate) {
		write_register_pc(cpu_state, read_register_pc(cpu_state) + INSTRUCTION_SIZE);
	}
}

/*
 * 4XNN
 * SINE VX NN
 * Skips the next instruction if VX is different from NN.
 */
void skip_if_different_from_immediate(CpuState *cpu_state, uint16_t instruction) {
	uint8_t r = extract_register_from_xnn(instruction);
	uint8_t r_value = read_register_bank(cpu_state, r);

	uint8_t immediate = extract_immediate_from_xnn(instruction);
	if (r_value != immediate) {
		write_register_pc(cpu_state, read_register_pc(cpu_state) + INSTRUCTION_SIZE);
	}
}

/*
 * 5XY0
 * SREQ VX VY
 * Skips the next instruction if VX is equal to VY.
 */
void skip_if_registers_equal(CpuState *cpu_state, uint16_t instruction) {
	uint8_t r1 = extract_first_register_from_xy(instruction);
	uint8_t r2 = extract_second_register_from_xy(instruction);

	uint8_t r1_val = read_register_bank(cpu_state, r1);
	uint8_t r2_val = read_register_bank(cpu_state, r2);

	if (r1_val == r2_val) {
		write_register_pc(cpu_state, read_register_pc(cpu_state) + INSTRUCTION_SIZE);
	}
}

/*
 * 9XY0
 * SRNE VX VY
 * Skips the next instruction if VX is different from VY.
 */
void skip_if_registers_different(CpuState *cpu_state, uint16_t instruction) {
	uint8_t r1 = extract_first_register_from_xy(instruction);
	uint8_t r2 = extract_second_register_from_xy(instruction);

	uint8_t r1_val = read_register_bank(cpu_state, r1);
	uint8_t r2_val = read_register_bank(cpu_state, r2);

	if (r1_val != r2_val) {
		write_register_pc(cpu_state, read_register_pc(cpu_state) + INSTRUCTION_SIZE);
	}
}

/*
 * EX9E
 * SKPR VX
 * Skip the next instruction if the key stored in VX is pressed.
 * VX should be 0x0 <= VX <= 0xF.
 */
void skip_pressed(CpuState *cpu_state, uint16_t instruction) {
	uint8_t vx = extract_register_from_x(instruction);
	uint8_t vx_val = read_register_bank(cpu_state, vx);

	bool key_pressed = is_key_pressed(cpu_state, vx_val);
	if (key_pressed) {
		write_register_pc(cpu_state, read_register_pc(cpu_state) + INSTRUCTION_SIZE);
	}
}

/*
 * EXA1
 * SKNP VX
 * Skip the next instruction if the key stored in VX is not pressed.
 * VX should be 0x0 <= VX <= 0xF.
 */
void skip_not_pressed(CpuState *cpu_state, uint16_t instruction) {
	uint8_t vx = extract_register_from_x(instruction);
	uint8_t vx_val = read_register_bank(cpu_state, vx);

	bool key_pressed = is_key_pressed(cpu_state, vx_val);
	if (!key_pressed) {
		write_register_pc(cpu_state, read_register_pc(cpu_state) + INSTRUCTION_SIZE);
	}

}

/* Registers */

/*
 * 8XY0
 * COPY VX VY
 * Set VX to the value of VY.
 */
void copy_register(CpuState *cpu_state, uint16_t instruction) {
	uint8_t vx = extract_first_register_from_xy(instruction);
	uint8_t vy = extract_second_register_from_xy(instruction);
	uint8_t vy_val = read_register_bank(cpu_state, vy);
	write_register_bank(cpu_state, vx, vy_val);
}

/*
 * 6XNN
 * SETR VX NN
 * Sets VX to NN.
 */
void set_register_to_immediate(CpuState *cpu_state, uint16_t instruction) {
	uint8_t r = extract_register_from_xnn(instruction);
	uint8_t immediate = extract_immediate_from_xnn(instruction);

	write_register_bank(cpu_state, r, immediate);
}

/*
 * ANNN
 * SETI NNN
 * Set the I register to NNN.
 */
void set_index_register(CpuState *cpu_state, uint16_t instruction) {
	uint16_t i_val = extract_immediate_from_nnn(instruction);
	write_index_register(cpu_state, i_val);
}

/* Memory */

/*
 * FX55
 * DUMP X
 * Write the contents of the registers from V0 through to VX to memory.
 * X is an immediate, not the register number from which to get a value from.
 * Valid values of X go from 0 (only V0 will be saved) to F (all registers will be saved).
 * The saved registers are written starting with V0 at I, up to VX at I+X.
 * If OPTION_DUMP_INCREMENTS_I, set I to one past the last written byte (I + X + 1).
 * Otherwise, do not alter I.
 */
void save_registers(CpuState *cpu_state, uint16_t instruction) {
	uint8_t x = extract_register_from_x(instruction);
	uint16_t base_address = read_index_register(cpu_state);
	for (uint8_t i = 0; i <= x; ++i) {
		uint16_t address = (base_address + i) & ADDRESS_BITMASK;
		uint8_t vi = read_register_bank(cpu_state, i);
		write_byte_memory(cpu_state, address, vi);
	}

#if OPTION_DUMP_INCREMENTS_I
	write_index_register(cpu_state, (base_address + x + 1) & ADDRESS_BITMASK);
#endif
}

/*
 * FX65
 * LOAD X
 * Read the registers V0 to VX from memory.
 * X is an immediate, not the register number from which to get a value from.
 * Valid values of X go from 0 (only V0 will be loaded) to F (all registers will be loaded).
 * Values are reading starting from I, up to I + X.
 */
void load_registers(CpuState *cpu_state, uint16_t instruction) {
	uint8_t x = extract_register_from_x(instruction);
	uint16_t base_address = read_index_register(cpu_state);
	for (uint8_t i = 0; i <= x; ++i) {
		uint16_t address = (base_address + i) & ADDRESS_BITMASK;
		uint8_t value = read_byte_memory(cpu_state, address);
		write_register_bank(cpu_state, i, value);
	}
}

/* Arithmetic */

/*
 * 7XNN
 * ADDI VX NN
 * Set VX to VX + NN. Do not set the carry flag on overflow.
 */
void add_immediate_to_register(CpuState *cpu_state, uint16_t instruction) {
	uint8_t r = extract_register_from_xnn(instruction);
	uint8_t r_val = read_register_bank(cpu_state, r);
	uint8_t immediate = extract_immediate_from_xnn(instruction);

	uint8_t result = r_val + immediate; // Ignore overflow

	write_register_bank(cpu_state, r, result);
}

/*
 * FX1E
 * IADD VX
 * Increment the index register by the value stored in VX.
 * If OPTION_OVERFLOW_ON_ADD_TO_INDEX,
 * set the carry flag if the result overflows over the size of the index register,
 * or reset it if it doesn't overflow.
 * Otherwise, do not alter the carry flag at all.
 */
void add_to_index(CpuState *cpu_state, uint16_t instruction) {
	uint8_t vx = extract_register_from_x(instruction);
	uint16_t i_val = read_index_register(cpu_state);

	uint16_t result = i_val + vx;

#if OPTION_OVERFLOW_ON_ADD_TO_INDEX
	uint8_t carry_flag;
	if ((~ADDRESS_BITMASK) & result) {
		carry_flag = 1;
	} else {
		carry_flag = 0;
	}
	write_register_bank(STATUS_REGISTER, carry_flag);
#endif

	write_index_register(cpu_state, result & ADDRESS_BITMASK);
}

/*
 * 7XNN
 * ADD VX VY
 * Set VX to VX + VY. Set the carry flag to 1 on overflow, otherwise set it to 0.
 */
void add_register_to_register(CpuState *cpu_state, uint16_t instruction) {
	uint8_t vx = extract_first_register_from_xy(instruction);
	uint8_t vy = extract_second_register_from_xy(instruction);

	uint8_t vx_val = read_register_bank(cpu_state, vx);
	uint8_t vy_val = read_register_bank(cpu_state, vy);

	// Need something bigger than a byte to check for overflow,
	// result will be trimmed when passed as argument
	uint16_t result = vx_val + vy_val;
	if (result > 255) {
		write_register_bank(cpu_state, STATUS_REGISTER, 1);
	} else {
		write_register_bank(cpu_state, STATUS_REGISTER, 0);
	}

	write_register_bank(cpu_state, vx, result);
}

/*
 * 8XY5
 * SUB VX VY
 * Set VX to VX - VY. Set the carry flag to 1 if VX > VY, otherwise set it to 0.
 */
void sub_register_from_register(CpuState *cpu_state, uint16_t instruction) {
	uint8_t vx = extract_first_register_from_xy(instruction);
	uint8_t vy = extract_second_register_from_xy(instruction);

	uint8_t vx_val = read_register_bank(cpu_state, vx);
	uint8_t vy_val = read_register_bank(cpu_state, vy);

	if (vx > vy) {
		write_register_bank(cpu_state, STATUS_REGISTER, 1);
	} else {
		write_register_bank(cpu_state, STATUS_REGISTER, 0);
	}

	uint8_t result = vx_val - vy_val;
	write_register_bank(cpu_state, vx, result);
}

/*
 * 8XY7
 * SUB- VX VY
 * Set VX to VY - VX. Set the carry flag to 1 if VY > VX, otherwise set it to 0.
 */
void negative_sub_register_from_register(CpuState *cpu_state, uint16_t instruction) {
	uint8_t vx = extract_first_register_from_xy(instruction);
	uint8_t vy = extract_second_register_from_xy(instruction);

	uint8_t vx_val = read_register_bank(cpu_state, vx);
	uint8_t vy_val = read_register_bank(cpu_state, vy);

	if (vy > vx) {
		write_register_bank(cpu_state, STATUS_REGISTER, 1);
	} else {
		write_register_bank(cpu_state, STATUS_REGISTER, 0);
	}

	uint8_t result = vy_val - vx_val;
	write_register_bank(cpu_state, vx, result);
}


/* FX33
 * DEC VX
 * Extract the digits of the decimal representation of the value stored in VX.
 * Store the hundreds digit in I, the tenths in I+1, and the units in I+2.
 */
void decimal_decode(CpuState *cpu_state, uint16_t instruction) {
	uint8_t vx = extract_register_from_x(instruction);
	uint8_t val = read_register_bank(cpu_state, vx);

	uint16_t address = read_index_register(cpu_state);

	for (int i = 2; i >= 0; --i) {
		uint8_t digit = val % 10;
		val /= 10;
		write_byte_memory(cpu_state, address + i, digit);
	}
}

/* Bit operations */

/*
 * 8XY1
 * OR VX VY
 * Set VX to the bitwise OR of VX and VY.
 */
void bitwise_or(CpuState *cpu_state, uint16_t instruction) {
	uint8_t vx = extract_first_register_from_xy(instruction);
	uint8_t vy = extract_second_register_from_xy(instruction);

	uint8_t vx_val = read_register_bank(cpu_state, vx);
	uint8_t vy_val = read_register_bank(cpu_state, vy);

	vx_val = vx_val | vy_val;
	write_register_bank(cpu_state, vx, vx_val);
}

/*
 * 8XY2
 * AND VX VY
 * Set VX to the bitwise AND of VX and VY.
 */
void bitwise_and(CpuState *cpu_state, uint16_t instruction) {
	uint8_t vx = extract_first_register_from_xy(instruction);
	uint8_t vy = extract_second_register_from_xy(instruction);

	uint8_t vx_val = read_register_bank(cpu_state, vx);
	uint8_t vy_val = read_register_bank(cpu_state, vy);

	vx_val = vx_val & vy_val;
	write_register_bank(cpu_state, vx, vx_val);
}

/*
 * 8XY3
 * XOR VX VY
 * Set VX to the bitwise XOR of VX and VY.
 */
void bitwise_xor(CpuState *cpu_state, uint16_t instruction) {
	uint8_t vx = extract_first_register_from_xy(instruction);
	uint8_t vy = extract_second_register_from_xy(instruction);

	uint8_t vx_val = read_register_bank(cpu_state, vx);
	uint8_t vy_val = read_register_bank(cpu_state, vy);

	vx_val = vx_val ^ vy_val;
	write_register_bank(cpu_state, vx, vx_val);
}

/*
 * 8XYE
 * SHIFTL VX VY.
 * If OPTION_USE_EXTRA_REGISTER_ON_SHIFT, set VX to VY << 1.
 * Otherwise, set VX to VX << 1, ignoring VY entirely.
 * In either case, set the carry flag to the value of the shifted out bit.
 */
void shift_left(CpuState *cpu_state, uint16_t instruction) {
	uint8_t vx = extract_first_register_from_xy(instruction);
	uint8_t value;

#if OPTION_USE_EXTRA_REGISTER_ON_SHIFT
	uint8_t vy = extract_second_register_from_xy(instruction);
	value = read_register_bank(cpu_state, vy);
#else
	value = read_register_bank(cpu_state, vx);
#endif

	uint8_t shifted_out_bit = (value & 0x80) >> 7;
	write_register_bank(cpu_state, STATUS_REGISTER, shifted_out_bit);

	uint8_t result = value << 1;
	write_register_bank(cpu_state, vx, result);
}

/*
 * 8XY6
 * SHIFTR VX VY.
 * If OPTION_USE_EXTRA_REGISTER_ON_SHIFT, set VX to VY >> 1.
 * Otherwise, set VX to VX >> 1, ignoring VY entirely.
 * In either case, set the carry flag to the value of the shifted out bit.
 */
void shift_right(CpuState *cpu_state, uint16_t instruction) {
	uint8_t vx = extract_first_register_from_xy(instruction);
	uint8_t value;

#if OPTION_USE_EXTRA_REGISTER_ON_SHIFT
	uint8_t vy = extract_second_register_from_xy(instruction);
	value = read_register_bank(cpu_state, vy);
#else
	value = read_register_bank(cpu_state,vx);
#endif

	uint8_t shifted_out_bit = value & 1;
	write_register_bank(cpu_state, STATUS_REGISTER, shifted_out_bit);

	uint8_t result = value >> 1;
	write_register_bank(cpu_state, vx, result);
}

/* Misc */

/*
 * CXNN
 * RAND VX NN
 * Generate a random byte, and perform a bitwise AND between it and NN.
 * Set VX to the result.
 */
void set_register_to_bitmasked_rand(CpuState *cpu_state, uint16_t instruction) {
	uint8_t vx = extract_register_from_xnn(instruction);
	uint8_t bitmask = extract_immediate_from_xnn(instruction);

	uint8_t random_byte = rand() & 0xF; // NOLINT(cert-msc50-cpp)
	uint8_t result = random_byte & bitmask;

	write_register_bank(cpu_state, vx, result);
}

/* I/O */

/*
 * FX07
 * RDEL VX
 * Set VX to the current value of the delay timer.
 */

void read_delay(CpuState *cpu_state, uint16_t instruction) {
	uint8_t vx = extract_register_from_x(instruction);
	uint8_t delay = read_delay_timer(cpu_state);

	write_register_bank(cpu_state, vx, delay);
}

/*
 * FX15
 * TDEL VX
 * Set the delay timer to the value of VX.
 */
void set_delay(CpuState *cpu_state, uint16_t instruction) {
	uint8_t vx = extract_register_from_x(instruction);
	uint8_t vx_val = read_register_bank(cpu_state, vx);

	write_delay_timer(cpu_state, vx_val);
}

/*
 * FX18
 * TSND VX
 * Set the sound timer to the value of VX.
 */
void set_sound(CpuState *cpu_state, uint16_t instruction) {
	uint8_t vx = extract_register_from_x(instruction);
	uint8_t vx_val = read_register_bank(cpu_state, vx);

	write_sound_timer(cpu_state, vx_val);
}

/*
 * FX0A
 * KEY VX
 * Depending on OPTION_KEY_WAIT_FOR_RELEASE, the instruction awaits until a key release (if set)
 * or a key release (if not set).
 * If a key event is happening right now, save the key that triggered the event in VX.
 * Otherwise, decrease the PC by 2.
 * This instruction will functionally repeat until a key event is triggered.
 */
void wait_for_key(CpuState *cpu_state, uint16_t instruction) {
	uint8_t key;
	if (any_key_pressed(cpu_state, &key)) {
		uint8_t vx = extract_register_from_x(instruction);
		write_register_bank(cpu_state, vx, key);
	} else {
		write_register_pc(cpu_state, read_register_pc(cpu_state) - INSTRUCTION_SIZE);
	}
}

/*
 * FX29
 * CHAR VX
 * Set the index register to the address where the sprite for the character in VX is located.
 */
void point_to_char(CpuState *cpu_state, uint16_t instruction) {
	uint8_t vx = extract_register_from_x(instruction);

	uint8_t requested_char = read_register_bank(cpu_state, vx) & 0x0F;
	uint16_t char_address = character_address(requested_char) & ADDRESS_BITMASK;

	write_index_register(cpu_state, char_address);
}