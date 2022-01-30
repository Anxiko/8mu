#include "registers.h"

uint16_t program_counter;
uint16_t index_register;
uint8_t bank[REGISTERS];

void initialize_registers() {
	write_register_pc(ROM_ADDRESS_START);
}

uint16_t read_register_pc() {
	return program_counter;
}

void write_register_pc(uint16_t v) {
	program_counter = v;
}

uint16_t read_index_register() {
	return index_register;
}

void write_index_register(uint16_t v) {
	index_register = v;
}

uint8_t read_register_bank(uint8_t r) {
	return bank[r];
}

void write_register_bank(uint8_t r, uint8_t v) {
	bank[r] = v;
}