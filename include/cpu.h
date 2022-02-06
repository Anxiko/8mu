#ifndef CHIP8_CPU_H
#define CHIP8_CPU_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "registers.h"
#include "instructions.h"
#include "memory.h"

typedef void Instruction(uint16_t);

uint16_t fetch();

Instruction *decode(uint16_t instruction);

void execute(uint16_t instruction, Instruction function);

#endif //CHIP8_CPU_H
