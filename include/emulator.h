#ifndef CHIP8_EMULATOR_H
#define CHIP8_EMULATOR_H

#include <stdint.h>

#include "state.h"
#include "screen.h"
#include "SDL.h"
#include "SDL_mixer.h"

#define PIXEL_SCALING 10
#define MIXER_CHANNEL 0

void render_display(CpuState *cpu_state, SDL_Renderer *renderer);

void play_beeper(CpuState *cpu_state, bool *previous_state, Mix_Chunk *beep_mix_chunk);

#endif //CHIP8_EMULATOR_H
