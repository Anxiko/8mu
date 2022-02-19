#ifndef CHIP8_BEEPER_H
#define CHIP8_BEEPER_H

#include <stdbool.h>
#include "SDL_mixer.h"

#include "state.h"

void set_beeper_state(CpuState *cpu_state, bool state);

#endif //CHIP8_BEEPER_H