#ifndef CHIP8_INSTRUCTIONS_H
#define CHIP8_INSTRUCTIONS_H

#include <string.h>
#include <stdint.h>
#include "stack.h"
#include "screen.h"
#include "registers.h"
#include "utils.h"
#include "memory.h"

#define INSTRUCTION_SIZE 2
#define VF_REGISTER ((uint8_t) 0xF)

#define SPRITE_WIDTH 8


/*
 * Graphics
 */

void clear_screen(uint16_t instruction);

void draw(uint16_t instruction);


/*
 * Jump/subroutines
 */

void jump(uint16_t instruction);

void jump_subroutine(uint16_t instruction);

void return_subroutine();

/*
 * Conditionals
 */

void skip_if_equal_to_immediate(uint16_t instruction);

void skip_if_different_from_immediate(uint16_t instruction);

void skip_if_registers_equal(uint16_t instruction);

void skip_if_registers_different(uint16_t instruction);

/*
 * Registers
 */

void set_register_to_immediate(uint16_t instruction);

void add_immediate_to_register(uint16_t instruction);

void set_index_register(uint16_t instruction);

#endif //CHIP8_INSTRUCTIONS_H
