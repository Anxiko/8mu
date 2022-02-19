#ifndef CHIP8_REGISTERS_H
#define CHIP8_REGISTERS_H

#include <stdint.h>
#include "memory.h"

void initialize_registers();

uint16_t read_register_pc();

void write_register_pc(uint16_t v);

uint16_t read_index_register();

void write_index_register(uint16_t v);

uint8_t read_register_bank(uint8_t r);

void write_register_bank(uint8_t r, uint8_t v);

#endif //CHIP8_REGISTERS_H
