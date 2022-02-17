#ifndef CHIP8_STATE_H
#define CHIP8_STATE_H

#include <stdint.h>

typedef struct CpuState {
	uint8_t *memory;
	uint16_t program_counter;
	uint16_t index_register;
	uint8_t *register_bank;
	uint8_t *display;

} CpuStateStruct;

#endif //CHIP8_STATE_H
