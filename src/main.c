#include "cpu.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <SDL3/SDL.h>

int main(int argc, char **argv) {
	// Setup a blank window
	if (!SDL_Init(SDL_INIT_VIDEO)) {
		fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
		return 1;
	}

	SDL_Window *window = SDL_CreateWindow("CHIP-8", 640, 320, 0);
	if (!window) {
		fprintf(stderr, "Failed to create window: %s\n", SDL_GetError());
		SDL_Quit();
		return 1;
	}

	SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);
	if (!renderer) {
		fprintf(stderr, "Failed to create renderer: %s\n", SDL_GetError());
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}

	// Setup The CPU
	Cpu cpu = cpu_new();

	// Load ROM
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <rom>\n", argv[0]);
		return 1;
	}

	FILE *fptr = fopen(argv[1], "rb");
	if (fptr == NULL) {
		printf("File does not exist");
		return 1;
	}

	int value;
	for (int i = 0x200; (value = fgetc(fptr)) != EOF; ++i) {
		cpu.memory[i] = value;
	}

	// Begin Main Loop

	bool running = true;
	SDL_Event event;

	const Uint64 FRAME_TIME = 1000 / 60;
	Uint64 last_frame = SDL_GetTicks();

	while (running) {
		cpu_step(&cpu);
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_EVENT_QUIT)
				running = false;
		}

		for (int i = 0; i < 10; i++) {
			cpu_step(&cpu);
		}

		if (SDL_GetTicks() - last_frame >= FRAME_TIME) {
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
			SDL_RenderPresent(renderer);
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

			for (int y = 0; y < 32; ++y) {
				for (int x = 0; x < 64; ++x) {
					int pixel = y * 64 + x;
					int byte = pixel >> 3;
					int bit = pixel & 7;

					if ((cpu.vram[byte] >> bit) & 1) {
						SDL_FRect rect = {x * 10, y * 10, 10, 10};

						SDL_RenderFillRect(renderer, &rect);
					}
				}
			}
			last_frame += FRAME_TIME;
		}

		SDL_Delay(1);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
