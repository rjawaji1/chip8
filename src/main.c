#include "SDL3/SDL_events.h"
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

		// Run every 60hz
		if (SDL_GetTicks() - last_frame >= FRAME_TIME) {
			// Decrement Timers
			if (cpu.st > 0)
				cpu.st--;
			if (cpu.dt > 0)
				cpu.dt--;

			// Clear Display
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
			SDL_RenderClear(renderer);

			// Render Display
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			for (int row = 0; row < 32; ++row) {
				for (int col = 0; col < 64; ++col) {
					int pixel = (row * 64) + col;
					int byte = pixel >> 3;
					int bit = pixel & 7;

					if ((cpu.vram[byte] >> (7 - bit)) & 1) {
						SDL_FRect rect = {col * 10, row * 10, 10, 10};
						SDL_RenderFillRect(renderer, &rect);
					}
				}
			}
			SDL_RenderPresent(renderer);

			// Set the next target frame
			last_frame += FRAME_TIME;
		}

		SDL_Delay(1);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
