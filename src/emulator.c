#include "emulator.h"

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