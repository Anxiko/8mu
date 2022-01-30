#ifndef CHIP8_SCREEN_H
#define CHIP8_SCREEN_H

#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32
#define SCREEN_SIZE_BYTES ((SCREEN_WIDTH * SCREEN_HEIGHT) / 8)

#include <stdint.h>

#endif //CHIP8_SCREEN_H

uint8_t *get_screen();

uint8_t read_pixel_from_screen(uint8_t x, uint8_t y);

void write_pixel_to_screen(uint8_t x, uint8_t y, uint8_t value);