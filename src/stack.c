#include "stack.h"

uint16_t stack_pop(CpuState *cpu_state) {
	uint16_t value = cpu_state->stack[--cpu_state->stack_size];
	return value;
}

void stack_push(CpuState *cpu_state, uint16_t v) {
	cpu_state->stack[cpu_state->stack_size++] = v;
}