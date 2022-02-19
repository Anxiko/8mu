#include "memory.h"

uint8_t read_byte_memory(CpuState *cpu_state, uint16_t address) {
	return cpu_state->memory[address];
}

uint16_t read_word_memory(CpuState *cpu_state, uint16_t address) {
	return read_word_from_array(cpu_state->memory, address);
}

void write_byte_memory(CpuState *cpu_state, uint16_t address, uint8_t value) {
	cpu_state->memory[address] = value;
}

void write_word_memory(CpuState *cpu_state, uint16_t address, uint16_t word) {
	write_word_to_array(cpu_state->memory, address, word);
}

uint16_t character_address(uint8_t c) {
	return FONT_ADDRESS_START + c * CHARACTER_HEIGHT;
}