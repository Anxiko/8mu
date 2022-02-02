#ifndef CHIP8_BEEPER_H
#define CHIP8_BEEPER_H

#define MIXER_CHANNEL 0

#include <stdbool.h>

#include "SDL_mixer.h"

#endif //CHIP8_BEEPER_H

void set_mixer_chunk(Mix_Chunk *mix_chunk);
void set_beeper_state(bool state);