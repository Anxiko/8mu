#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define SDL_MAIN_HANDLED

#include "cpu.h"
#include "debug.h"
#include "emulator.h"

#include "SDL.h"
#include "SDL_mixer.h"

#define MIXER_CHUNK_SIZE 1024
#define MIXER_CHANNELS_REQUESTED 1
#define ENV_VOLUME "CHIP8_VOLUME"


uint8_t rom[ROM_SIZE];
CpuState cpu_state;

void quit_on_sdl_error(bool error, const char *error_msg);

void set_volume();

int main(int argc, const char *argv[]) {
	if (argc != 2) {
		fprintf(stderr, "Invalid number of arguments\n");
		printf("Usage: 8mu path/to/chip8_rom.ch8\n");
		return EXIT_FAILURE;
	}

	const char *rom_path = argv[1];

	SDL_SetMainReady();
	quit_on_sdl_error(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0, "Failed to initialize SDL");

	quit_on_sdl_error(Mix_OpenAudio(
		MIX_DEFAULT_FREQUENCY,
		MIX_DEFAULT_FORMAT,
		MIX_DEFAULT_CHANNELS,
		MIXER_CHUNK_SIZE
	) < 0, "Could not open audio");
	quit_on_sdl_error(
		Mix_AllocateChannels(MIXER_CHANNELS_REQUESTED) < 0,
		"Failed to allocate channels"
	);
	Mix_Chunk *mix_chunk = Mix_LoadWAV("data/beep.wav");
	if (mix_chunk == NULL) {
		fprintf(stderr, "Failed to load sound effect");
		exit(EXIT_FAILURE);
	}
	Mix_Volume(MIXER_CHANNEL, 1);
	set_volume();

	SDL_Window *window = SDL_CreateWindow(
		"Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		SCREEN_WIDTH * PIXEL_SCALING, SCREEN_HEIGHT * PIXEL_SCALING, 0
	);
	quit_on_sdl_error(window == NULL, "Failed to create window: %s");

	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	quit_on_sdl_error(renderer == NULL, "Failed to create renderer");

	memset(rom, 0, ROM_SIZE);
	FILE *file_ptr = fopen(rom_path, "rb");
	if (file_ptr == NULL) {
		fprintf(stderr, "Failed to open file %s", rom_path);
		return EXIT_FAILURE;
	}
	size_t bytes_read = fread(rom, 1, ROM_SIZE, file_ptr);
	printf("Read %zu byte(s)\n", bytes_read);
	fclose(file_ptr);

	init_state(&cpu_state, rom);
	bool current_sound_state = false;

	bool running = true;
	SDL_Event e;

	while (running) {
		uint16_t instruction = fetch(&cpu_state);
		Instruction *function = decode(instruction);
		execute(&cpu_state, instruction, function);

		// print_display();
		render_display(&cpu_state, renderer);
		SDL_UpdateWindowSurface(window);

		while (SDL_PollEvent(&e)) {
			switch (e.type) {
				case SDL_QUIT: {
					running = false;
					break;
				}
			}
		}

		update_beeper_status(&cpu_state);
		update_keyboard_state(&cpu_state);
		play_beeper(&cpu_state, &current_sound_state, mix_chunk);

		SDL_Delay(2);
	}

	Mix_FreeChunk(mix_chunk);
	Mix_CloseAudio();

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return EXIT_SUCCESS;
}

void quit_on_sdl_error(bool error, const char *error_msg) {
	if (error) {
		printf("%s: %s", error_msg, SDL_GetError());
		exit(EXIT_FAILURE);
	}
}

void set_volume() {
	char const *env = getenv(ENV_VOLUME);

	if (env != NULL) {
		char *end_ptr = NULL;
		long volume = strtol(env, &end_ptr, 10);
		if (end_ptr != env) {
			Mix_Volume(MIXER_CHANNEL, volume);
		}

	}
}