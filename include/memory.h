#ifndef CHIP8_MEMORY_H
#define CHIP8_MEMORY_H

#define CHARACTER_HEIGHT 5
#define NUMBER_OF_CHARACTERS 16

#define ROM_ADDRESS_START 512
#define FONT_ADDRESS_START 0x0050
#define ROM_SIZE (MEMORY_SIZE - ROM_ADDRESS_START)

#include <stdint.h>
#include <string.h>
#include "utils.h"

void initialize_memory(const uint8_t *rom);

uint8_t read_byte_memory(uint16_t address);

uint16_t read_word_memory(uint16_t address);

void write_byte_memory(uint16_t address, uint8_t value);

void write_word_memory(uint16_t address, uint16_t word);

uint16_t character_address(uint8_t c);

#endif //CHIP8_MEMORY_H
