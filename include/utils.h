#ifndef CHIP8_UTILS_H
#define CHIP8_UTILS_H

#define ADDRESS_INSTRUCTION_BITMASK ((uint16_t) 0x0FFF)

#define INSTRUCTION_FIELD_ADDRESS_NNN_BITMASK ((uint16_t) 0x0FFF)
#define INSTRUCTION_FIELD_ADDRESS_NNN_OFFSET 0

#define INSTRUCTION_FIELD_REGISTER_XNN_BITMASK ((uint16_t) 0x0F00)
#define INSTRUCTION_FIELD_REGISTER_XNN_OFFSET 8

#define INSTRUCTION_FIELD_IMMEDIATE_XNN_BITMASK ((uint16_t) 0x00FF)
#define INSTRUCTION_FIELD_IMMEDIATE_XNN_OFFSET 0

#define INSTRUCTION_FIELD_FIRST_REGISTER_XY_BITMASK ((uint16_t) 0x0F00)
#define INSTRUCTION_FIELD_FIRST_REGISTER_XY_OFFSET 8

#define INSTRUCTION_FIELD_SECOND_REGISTER_XY_BITMASK ((uint16_t) 0x00F0)
#define INSTRUCTION_FIELD_SECOND_REGISTER_XY_OFFSET 4

#define INSTRUCTION_FIELD_FIRST_REGISTER_XYN_BITMASK ((uint16_t) 0x0F00)
#define INSTRUCTION_FIELD_FIRST_REGISTER_XYN_OFFSET 8

#define INSTRUCTION_FIELD_FIRST_REGISTER_XYN_BITMASK ((uint16_t) 0x0F00)
#define INSTRUCTION_FIELD_FIRST_REGISTER_XYN_OFFSET 8

#define INSTRUCTION_FIELD_SECOND_REGISTER_XYN_BITMASK ((uint16_t) 0x00F0)
#define INSTRUCTION_FIELD_SECOND_REGISTER_XYN_OFFSET 4

#define INSTRUCTION_FIELD_IMMEDIATE_XYN_BITMASK ((uint16_t) 0x000F)
#define INSTRUCTION_FIELD_IMMEDIATE_XYN_OFFSET 0


#include <stdint.h>

uint16_t read_word_from_array(uint8_t *ptr, intptr_t offset_in_bytes);

void write_word_to_array(uint8_t *ptr, intptr_t offset_in_bytes, uint16_t value);

uint8_t extract_register_from_xnn(uint16_t instruction);

uint8_t extract_immediate_from_xnn(uint16_t instruction);

uint8_t extract_first_register_from_xy(uint16_t instruction);

uint8_t extract_second_register_from_xy(uint16_t instruction);

uint16_t extract_immediate_from_nnn(uint16_t instruction);

uint8_t extract_first_register_from_xyn(uint16_t instruction);

uint8_t extract_second_register_from_xyn(uint16_t instruction);

uint8_t extract_immediate_from_xyn(uint16_t instruction);

#endif //CHIP8_UTILS_H
