#ifndef CHIP8_STATE_H
#define CHIP8_STATE_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
	uint8_t *memory;
	uint16_t program_counter;
	uint16_t index_register;
	uint8_t *register_bank;
	uint16_t *stack;
	uint8_t stack_size;
	uint8_t *display;
	bool sound_playing;

} CpuState;

#endif //CHIP8_STATE_H
