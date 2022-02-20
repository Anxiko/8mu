#include "cpu.h"


uint16_t fetch(CpuState *cpu_state) {
	uint16_t pc = read_register_pc(cpu_state);
	write_register_pc(cpu_state, pc + INSTRUCTION_SIZE);

	return read_word_memory(cpu_state, pc);
}

Instruction *decode(uint16_t instruction) {
	switch ((instruction & 0xF000) >> 12) {
		case 0x0: {
			switch (instruction & 0x0FFF) {
				case 0xE0:
					return clear_screen;
				case 0xEE:
					return return_subroutine;
			}
			break;
		}

		case 0x1:
			return jump;
		case 0x2:
			return jump_subroutine;
		case 0x3:
			return skip_if_equal_to_immediate;
		case 0x4:
			return skip_if_different_from_immediate;
		case 0x5:
			return skip_if_registers_equal;
		case 0x6:
			return set_register_to_immediate;
		case 0x7:
			return add_immediate_to_register;

		case 0x8: {
			switch (instruction & 0xF) {
				case 0x0:
					return copy_register;
				case 0x1:
					return bitwise_or;
				case 0x2:
					return bitwise_and;
				case 0x3:
					return bitwise_xor;
				case 0x4:
					return add_register_to_register;
				case 0x5:
					return sub_register_from_register;
				case 0x6:
					return shift_right;
				case 0x7:
					return negative_sub_register_from_register;
				case 0xE:
					return shift_left;
			}
			break;
		}

		case 0x9:
			return skip_if_registers_different;
		case 0xA:
			return set_index_register;
		case 0xB:
			return jump_with_offset;
		case 0xC:
			return set_register_to_bitmasked_rand;
		case 0xD:
			return draw;

		case 0xE: {
			switch (instruction & 0xFF) {
				case 0x9E: return skip_pressed;
				case 0xA1: return skip_not_pressed;
			}
			break;
		}

		case 0xF: {
			switch (instruction & 0xFF) {
				case 0x07: return read_delay;
				case 0x0A: return wait_for_key;
				case 0x15: return set_delay;
				case 0x18: return set_sound;
				case 0x1E: return add_to_index;
				case 0x29: return point_to_char;
				case 0x33: return decimal_decode;
				case 0x55: return save_registers;
				case 0x65: return load_registers;
			}
			break;
		}
	}

	return NULL;
}

void execute(CpuState *cpu_state, uint16_t instruction, Instruction function) {
	if (function == NULL) {
		fprintf(stderr, "Could not decode instruction: %X", instruction);
		exit(EXIT_FAILURE);
	}
	function(cpu_state, instruction);
}