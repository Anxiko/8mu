#include "screen.h"

uint8_t display[SCREEN_SIZE_BYTES];

uint8_t *get_screen() {
	return display;
}

uint8_t read_pixel_from_screen(uint8_t x, uint8_t y) {
	uintptr_t pixel_address = y * SCREEN_WIDTH + x;
	uintptr_t pixel_byte_address = pixel_address / 8;
	uintptr_t pixel_offset_in_byte = pixel_address % 8;

	uint8_t pixel_byte = display[pixel_byte_address];
	return (pixel_byte >> pixel_offset_in_byte) & 0x1;
}

void write_pixel_to_screen(uint8_t x, uint8_t y, uint8_t value) {
	uintptr_t pixel_address = y * SCREEN_WIDTH + x;
	uintptr_t pixel_byte_address = pixel_address / 8;
	uintptr_t pixel_offset_in_byte = pixel_address % 8;

	uint8_t pixel_byte = display[pixel_byte_address];
	uint8_t mask = ~(1 << pixel_offset_in_byte);
	pixel_byte = (pixel_byte & mask) | (value << pixel_offset_in_byte);
	display[pixel_byte_address] = pixel_byte;
}