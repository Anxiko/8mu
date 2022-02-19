#include "keyboard.h"

bool is_key_pressed(CpuState *cpu_state, uint8_t key) {
	return cpu_state->keyboard[key];
}

bool any_key_pressed(CpuState *cpu_state, uint8_t *pressed_key) {
	for (uint8_t key = 0; key < NUMBER_OF_KEYS; ++key) {
		if (is_key_pressed(cpu_state, key)) {
			*pressed_key = key;
			return true;
		}
	}

	return false;
}