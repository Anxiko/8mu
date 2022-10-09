#include "emulator.h"

const SDL_Keycode KEYBOARD_CODES[NUMBER_OF_KEYS] = {
	SDL_SCANCODE_X,
	SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3,
	SDL_SCANCODE_Q, SDL_SCANCODE_W, SDL_SCANCODE_E,
	SDL_SCANCODE_A, SDL_SCANCODE_S, SDL_SCANCODE_D,
	SDL_SCANCODE_Z, SDL_SCANCODE_C,
	SDL_SCANCODE_4, SDL_SCANCODE_R, SDL_SCANCODE_F, SDL_SCANCODE_V
};

void render_display(CpuState *cpu_state, SDL_Renderer *renderer) {
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_Rect rect = {0, 0, PIXEL_SCALING, PIXEL_SCALING};

	for (int x = 0; x < SCREEN_WIDTH; ++x) {
		for (int y = 0; y < SCREEN_HEIGHT; ++y) {
			uint8_t pixel = read_pixel_from_screen(cpu_state, x, y);
			if (pixel) {
				rect.x = x * PIXEL_SCALING;
				rect.y = y * PIXEL_SCALING;
				SDL_RenderFillRect(renderer, &rect);
			}
		}
	}
	SDL_RenderPresent(renderer);
}

void play_beeper(CpuState *cpu_state, bool *previous_state, Mix_Chunk *beep_mix_chunk) {
	/*
	 * We only need to do something when the state changes.
	 * If we go from not playing to playing, we start playing the beep sound in a channel looping.
	 * If we were playing and stop playing, we stop the channel.
	 */
	if (cpu_state->sound_playing == *previous_state) {
		return;
	}

	if (cpu_state->sound_playing) {
		Mix_PlayChannel(MIXER_CHANNEL, beep_mix_chunk, -1);
	} else {
		Mix_HaltChannel(MIXER_CHANNEL);
	}
	*previous_state = cpu_state->sound_playing;
}

void update_keyboard_state(CpuState *cpu_state) {
	const Uint8 *sdl_keyboard_state = SDL_GetKeyboardState(NULL);

	for (uint8_t key = 0; key < NUMBER_OF_KEYS; ++key) {
		SDL_Keycode keycode = KEYBOARD_CODES[key];
		if (sdl_keyboard_state[keycode]) {
			set_key_pressed(cpu_state, key, true);
		} else {
			set_key_pressed(cpu_state, key, false);
		}
	}
}