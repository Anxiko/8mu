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
#define STATUS_REGISTER ((uint8_t) 0xF)

/*
 * If set, force the shift operations SHIFTL and SHIFTR to use the extra provided.
 * Otherwise, perform the shift in place, ignoring the extra argument entirely.
 */
#define OPTION_USE_EXTRA_REGISTER_ON_SHIFT 1

/*
 * Controls how the destination address for JUMPR.
 * If set, implement BXNN: the base register is VX, and the offset is NN.
 * Otherwise, implement BNNN: the base register is always V0, and NNN is the offset.
 */
#define OPTION_REGISTER_ARGUMENT_ON_JUMP_WITH_OFFSET 0

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
