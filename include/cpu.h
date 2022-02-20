#ifndef CHIP8_CPU_H
#define CHIP8_CPU_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "state.h"

#include "registers.h"
#include "instructions.h"
#include "memory.h"

typedef void Instruction(CpuState *, uint16_t);

uint16_t fetch(CpuState *cpu_state);

Instruction *decode(uint16_t instruction);

void execute(CpuState *cpu_state, uint16_t instruction, Instruction function);

#endif //CHIP8_CPU_H
