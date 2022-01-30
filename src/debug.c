#include "debug.h"

void print_separator() {
	for (int x = 0; x < SCREEN_WIDTH; ++x) {
		printf("=");
	}
	printf("\n");
}

void print_display() {
	print_separator();
	for (int y = 0; y < SCREEN_HEIGHT; ++y) {
		for (int x = 0; x < SCREEN_WIDTH; ++x) {
			uint8_t pixel = read_pixel_from_screen(x, y);
			if (pixel) {
				printf("X");
			} else {
				printf("_");
			}
		}
		printf("\n");
	}
	print_separator();
}