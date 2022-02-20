#include "registers.h"

uint16_t read_register_pc(CpuState *cpu_state) {
	return cpu_state->program_counter;
}

void write_register_pc(CpuState *cpu_state, uint16_t v) {
	cpu_state->program_counter = v;
}

uint16_t read_index_register(CpuState *cpu_state) {
	return cpu_state->index_register;
}

void write_index_register(CpuState *cpu_state, uint16_t v) {
	cpu_state->index_register = v;
}

uint8_t read_register_bank(CpuState *cpu_state, uint8_t r) {
	return cpu_state->register_bank[r];
}

void write_register_bank(CpuState *cpu_state, uint8_t r, uint8_t v) {
	cpu_state->register_bank[r] = v;
}