#ifndef CHIP8_STACK_H
#define CHIP8_STACK_H

#define STACK_SIZE 16

#include <stdint.h>

#include "state.h"

uint16_t stack_pop(CpuState *cpu_state);

void stack_push(CpuState *cpu_state, uint16_t v);

#endif //CHIP8_STACK_H
