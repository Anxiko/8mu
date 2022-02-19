#include "beeper.h"

void set_beeper_state(CpuState *cpu_state, bool state) {
	cpu_state->sound_playing = state;
}

