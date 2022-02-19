#ifndef CHIP8_STATE_H
#define CHIP8_STATE_H

#define MEMORY_SIZE (4 * 1024)
#define STACK_SIZE 16
#define REGISTERS 16

#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32
#define SCREEN_SIZE_BYTES ((SCREEN_WIDTH * SCREEN_HEIGHT) / 8)

#define NUMBER_OF_KEYS 16

#include <stdint.h>
#include <stdbool.h>

typedef struct {
	int64_t set_ts_millis;
	uint8_t set_value;
} TimerRegister;

typedef struct {
	// Memory
	uint8_t memory[MEMORY_SIZE];
	uint16_t stack[STACK_SIZE];
	uint8_t stack_size;

	// Registers
	uint16_t program_counter;
	uint16_t index_register;
	uint8_t register_bank[REGISTERS];

	// Sound & video
	uint8_t display[SCREEN_SIZE_BYTES];
	bool sound_playing;

	// Keyboard
	bool keyboard[NUMBER_OF_KEYS];

	// Timers
	TimerRegister delay_timer;
	TimerRegister sound_timer;
} CpuState;

#endif //CHIP8_STATE_H
