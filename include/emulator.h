#ifndef CHIP8_EMULATOR_H
#define CHIP8_EMULATOR_H

#include <stdint.h>

#include "screen.h"
#include "SDL.h"

#define PIXEL_SCALING 10

void render_display(SDL_Renderer *renderer);

#endif //CHIP8_EMULATOR_H
