#ifndef CHIP8_MEMORY_H
#define CHIP8_MEMORY_H

#include <stdint.h>
#include <string.h>

#include "state.h"
#include "utils.h"

uint8_t read_byte_memory(CpuState *cpu_state, uint16_t address);

uint16_t read_word_memory(CpuState *cpu_state, uint16_t address);

void write_byte_memory(CpuState *cpu_state, uint16_t address, uint8_t value);

void write_word_memory(CpuState *cpu_state, uint16_t address, uint16_t word);

uint16_t character_address(uint8_t c);

#endif //CHIP8_MEMORY_H
