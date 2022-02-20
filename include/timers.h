#ifndef CHIP8_TIMERS_H
#define CHIP8_TIMERS_H

#include "state.h"
#include "beeper.h"
#include "time_millis.h"

uint8_t read_delay_timer(CpuState *cpu_state);

void write_delay_timer(CpuState *cpu_state, uint8_t delay);

void write_sound_timer(CpuState *cpu_state, uint8_t delay);

void refresh_timers(CpuState *cpu_state);

#endif //CHIP8_TIMERS_H
