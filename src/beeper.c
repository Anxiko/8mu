#include "beeper.h"

Mix_Chunk *beep_mix_chunk = NULL;
bool playing = false;

void set_mixer_chunk(Mix_Chunk *mix_chunk) {
	beep_mix_chunk = mix_chunk;
}

void set_beeper_state(bool state) {
	if (state == playing) {
		return;
	}

	if (state) {
		Mix_PlayChannel(MIXER_CHANNEL, beep_mix_chunk, -1);
	} else {
		Mix_HaltChannel(MIXER_CHANNEL);
	}
	playing = state;
}

