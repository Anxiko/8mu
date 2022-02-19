#ifndef CHIP8_KEYBOARD_H
#define CHIP8_KEYBOARD_H

#include <stdbool.h>

#include "SDL_keycode.h"
#include "SDL_keyboard.h"

#include "state.h"

bool is_key_pressed(CpuState *cpu_state, uint8_t key);

bool any_key_pressed(CpuState  *cpu_state, uint8_t *pressed_key);

#endif //CHIP8_KEYBOARD_H
