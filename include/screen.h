#ifndef CHIP8_SCREEN_H
#define CHIP8_SCREEN_H

#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32
#define SCREEN_SIZE_BYTES ((SCREEN_WIDTH * SCREEN_HEIGHT) / 8)

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "state.h"

#define COLOR_BLACK false
#define COLOR_WHITE true

#endif //CHIP8_SCREEN_H

void fill_screen(CpuState *cpu_state, bool color);

uint8_t read_pixel_from_screen(CpuState *cpu_state, uint8_t x, uint8_t y);

void write_pixel_to_screen(CpuState *cpu_state, uint8_t x, uint8_t y, uint8_t value);