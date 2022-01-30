#ifndef CHIP8_STACK_H
#define CHIP8_STACK_H

#define STACK_SIZE 16

#include <stdint.h>

uint16_t stack_pop();

void stack_push(uint16_t v);

#endif //CHIP8_STACK_H
