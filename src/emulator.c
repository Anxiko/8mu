#include "emulator.h"

void render_display(SDL_Renderer *renderer) {
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_Rect rect = {0, 0, PIXEL_SCALING, PIXEL_SCALING};

	for (int x = 0; x < SCREEN_WIDTH; ++x) {
		for (int y = 0; y < SCREEN_HEIGHT; ++y) {
			uint8_t pixel = read_pixel_from_screen(x, y);
			if (pixel) {
				rect.x = x * PIXEL_SCALING;
				rect.y = y * PIXEL_SCALING;
				SDL_RenderFillRect(renderer, &rect);
			}
		}
	}
	SDL_RenderPresent(renderer);
}