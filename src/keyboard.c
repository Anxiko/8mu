#include "keyboard.h"

const SDL_Keycode KEYBOARD_CODES[NUMBER_OF_KEYS] = {
	SDL_SCANCODE_X,
	SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3,
	SDL_SCANCODE_Q, SDL_SCANCODE_W, SDL_SCANCODE_E,
	SDL_SCANCODE_A, SDL_SCANCODE_S, SDL_SCANCODE_D,
	SDL_SCANCODE_Z, SDL_SCANCODE_C,
	SDL_SCANCODE_4, SDL_SCANCODE_R, SDL_SCANCODE_F, SDL_SCANCODE_V
};

bool is_key_pressed(uint8_t key) {
	SDL_Keycode keycode = KEYBOARD_CODES[key];
	const Uint8 *state = SDL_GetKeyboardState(NULL);

	return state[keycode] ? true : false;
}

bool any_key_pressed(uint8_t *pressed_key) {
	for (uint8_t key = 0; key < NUMBER_OF_KEYS; ++key) {
		if (is_key_pressed(key)) {
			*pressed_key = key;
			return true;
		}
	}
	return false;
}