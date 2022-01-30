#include "cpu.h"


uint16_t fetch() {
	uint16_t pc = read_register_pc();
	write_register_pc(pc + INSTRUCTION_SIZE);

	return read_word_memory(pc);
}

uint8_t decode(uint16_t instruction) {
	return (uint8_t) ((instruction & DECODE_BITMASK) >> DECODE_OFFSET);
}

void execute(uint16_t instruction, uint8_t decoded_instruction) {
	switch (decoded_instruction) {
		case 0x0: {
			switch (instruction) {
				case 0x00E0: {
					clear_screen(instruction);
					break;
				}
			}
			break;
		}
		case 0x1: {
			jump(instruction);
			break;
		}
		case 0x6: {
			set_register_to_immediate(instruction);
			break;
		}
		case 0x7: {
			add_immediate_to_register(instruction);
			break;
		}
		case 0xA: {
			set_index_register(instruction);
			break;
		}
		case 0xD: {
			draw(instruction);
			break;
		}
	}
}