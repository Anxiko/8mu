#ifndef CHIP8_TIMERS_H
#define CHIP8_TIMERS_H

#include <time.h>
#include <stdint.h>

#include "beeper.h"

uint8_t read_delay_timer();

void write_delay_timer(uint8_t delay);

void write_sound_timer(uint8_t delay);

#endif //CHIP8_TIMERS_H
