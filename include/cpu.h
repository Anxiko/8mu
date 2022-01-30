#ifndef CHIP8_CPU_H
#define CHIP8_CPU_H

#include <stdint.h>
#include "registers.h"
#include "instructions.h"
#include "memory.h"

uint16_t fetch();

uint8_t decode(uint16_t instruction);

void execute(uint16_t instruction, uint8_t decoded_instruction);

#endif //CHIP8_CPU_H
