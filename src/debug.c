#include "debug.h"

void print_separator() {
	for (int x = 0; x < SCREEN_WIDTH; ++x) {
		printf("=");
	}
	printf("\n");
}

void print_display(CpuState *cpu_state) {
	print_separator();
	for (int y = 0; y < SCREEN_HEIGHT; ++y) {
		for (int x = 0; x < SCREEN_WIDTH; ++x) {
			uint8_t pixel = read_pixel_from_screen(cpu_state, x, y);
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