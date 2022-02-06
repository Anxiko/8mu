#ifndef CHIP8_INSTRUCTIONS_H
#define CHIP8_INSTRUCTIONS_H

#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "stack.h"
#include "screen.h"
#include "registers.h"
#include "utils.h"
#include "memory.h"
#include "keyboard.h"
#include "timers.h"

#define INSTRUCTION_SIZE 2
#define STATUS_REGISTER ((uint8_t) 0xF)
#define SPRITE_WIDTH 8

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

/*
 * If set, the instruction IADD sets the carry flag if the result overflows over the expected 12 bits size.
 * This includes setting the flag to 0 if it doesn't overflow.
 * If not set, the instruction will not alter the carry flag whatsoever.
 */
#define OPTION_OVERFLOW_ON_ADD_TO_INDEX 0

/*
 * If set, DUMP will set the register I to I + X + 1 after its execution.
 * Otherwise, DUMP will not alter the I.
 */
#define OPTION_DUMP_INCREMENTS_I 0

/* Graphics */

void clear_screen(uint16_t instruction);

void draw(uint16_t instruction);

/* Jumping and subroutines */

void jump(uint16_t instruction);

void jump_subroutine(uint16_t instruction);

void jump_with_offset(uint16_t instruction);

void return_subroutine(uint16_t);

/* Conditionals */

void skip_if_equal_to_immediate(uint16_t instruction);

void skip_if_different_from_immediate(uint16_t instruction);

void skip_if_registers_equal(uint16_t instruction);

void skip_if_registers_different(uint16_t instruction);

void skip_pressed(uint16_t instruction);

void skip_not_pressed(uint16_t instruction);

/* Registers */

void copy_register(uint16_t instruction);

void set_register_to_immediate(uint16_t instruction);

void set_index_register(uint16_t instruction);

/* Memory */

void save_registers(uint16_t instruction);

void load_registers(uint16_t instruction);

/* Arithmetic */

void add_immediate_to_register(uint16_t instruction);

void add_to_index(uint16_t instruction);

void add_register_to_register(uint16_t instruction);

void sub_register_from_register(uint16_t instruction);

void negative_sub_register_from_register(uint16_t instruction);

void decimal_decode(uint16_t instruction);

/* Bit operations */

void bitwise_or(uint16_t instruction);

void bitwise_and(uint16_t instruction);

void bitwise_xor(uint16_t instruction);

void shift_left(uint16_t instruction);

void shift_right(uint16_t instruction);

/* Misc */

void set_register_to_bitmasked_rand(uint16_t instruction);

/* I/O */

void read_delay(uint16_t instruction);

void set_delay(uint16_t instruction);

void set_sound(uint16_t instruction);

void wait_for_key(uint16_t instruction);

void point_to_char(uint16_t instruction);