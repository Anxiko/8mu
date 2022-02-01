#ifndef CHIP8_KEYBOARD_H
#define CHIP8_KEYBOARD_H

#include <stdbool.h>

#include "SDL_keycode.h"
#include "SDL_keyboard.h"

#define NUMBER_OF_KEYS 16

bool is_key_pressed(uint8_t key);

#endif //CHIP8_KEYBOARD_H
