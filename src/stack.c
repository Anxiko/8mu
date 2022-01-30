#include "stack.h"

uint16_t STACK[STACK_SIZE];
uint8_t size = 0;

uint16_t stack_pop() {
	uint16_t value = STACK[--size];
	return value;
}

void stack_push(uint16_t v) {
	STACK[size++] = v;
}