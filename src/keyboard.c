#include "keyboard.h"

const SDL_Keycode KEYBOARD_CODES[NUMBER_OF_KEYS] = {
	SDLK_x,
	SDLK_1, SDLK_2, SDLK_3,
	SDLK_q, SDLK_w, SDLK_e,
	SDLK_a, SDLK_s, SDLK_d,
	SDLK_z, SDLK_c,
	SDLK_4, SDLK_r, SDLK_f, SDLK_v
};

bool is_key_pressed(uint8_t key) {
	SDL_Keycode keycode = KEYBOARD_CODES[key];
	const Uint8 *state = SDL_GetKeyboardState(NULL);

	return state[keycode] ? true : false;
}