#ifndef CHIP8_REGISTERS_H
#define CHIP8_REGISTERS_H

#include <stdint.h>

#include "state.h"
#include "memory.h"

uint16_t read_register_pc(CpuState *cpu_state);

void write_register_pc(CpuState *cpu_state, uint16_t v);

uint16_t read_index_register(CpuState *cpu_state);

void write_index_register(CpuState *cpu_state, uint16_t v);

uint8_t read_register_bank(CpuState *cpu_state, uint8_t r);

void write_register_bank(CpuState *cpu_state, uint8_t r, uint8_t v);

#endif //CHIP8_REGISTERS_H
