#include "SDL3/SDL_audio.h"
#include "cpu.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <SDL3/SDL.h>

#define SAMPLE_RATE 44100
#define BUFFER_SIZE 2048
#define FREQUENCY 440.0f
#define AMPLITUDE 0.1f

int main(int argc, char **argv) {
	// Initialize audio and video
	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
		fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
		return 1;
	}

	SDL_Window *window = SDL_CreateWindow("CHIP-8", 640, 350, 0);
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

	SDL_AudioSpec src_spec;
	src_spec.format = SDL_AUDIO_F32;
	src_spec.channels = 1;
	src_spec.freq = SAMPLE_RATE;

	SDL_AudioStream *stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &src_spec, NULL, NULL);
	if (!stream) {
		SDL_Log("Stream creation failed: %s", SDL_GetError());
		SDL_Quit();
		return -1;
	}

	SDL_ResumeAudioStreamDevice(stream);

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

	const Uint64 DISPLAY_FRAME_TIME = 1000 / 60; // 60hz
	const Uint64 CPU_FRAME_TIME = 1000 / 700;	 // 500hz

	Uint64 last_display_frame = SDL_GetTicks();
	Uint64 last_cpu_frame = SDL_GetTicks();

	float audio_buffer[BUFFER_SIZE];
	float audio_phase = 0;
	float audio_volume = 1.0f; // Keep it comfortable
	float audio_frequency = 440.0f;

	SDL_ResumeAudioStreamDevice(stream);

	while (running) {
		cpu_step(&cpu);
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_EVENT_QUIT:
				running = false;
				break;

			case SDL_EVENT_KEY_DOWN:
				// clang-format off
				switch (event.key.key) {
				case SDLK_1: cpu.keyboard_state |= CHIP8_KEYCODE_1; break;
				case SDLK_2: cpu.keyboard_state |= CHIP8_KEYCODE_2; break;
				case SDLK_3: cpu.keyboard_state |= CHIP8_KEYCODE_3; break;
				case SDLK_4: cpu.keyboard_state |= CHIP8_KEYCODE_C; break;
				case SDLK_Q: cpu.keyboard_state |= CHIP8_KEYCODE_4; break;
				case SDLK_W: cpu.keyboard_state |= CHIP8_KEYCODE_5; break;
				case SDLK_E: cpu.keyboard_state |= CHIP8_KEYCODE_6; break;
				case SDLK_R: cpu.keyboard_state |= CHIP8_KEYCODE_D; break;
				case SDLK_A: cpu.keyboard_state |= CHIP8_KEYCODE_7; break;
				case SDLK_S: cpu.keyboard_state |= CHIP8_KEYCODE_8; break;
				case SDLK_D: cpu.keyboard_state |= CHIP8_KEYCODE_9; break;
				case SDLK_F: cpu.keyboard_state |= CHIP8_KEYCODE_E; break;
				case SDLK_Z: cpu.keyboard_state |= CHIP8_KEYCODE_A; break;
				case SDLK_X: cpu.keyboard_state |= CHIP8_KEYCODE_0; break;
				case SDLK_C: cpu.keyboard_state |= CHIP8_KEYCODE_B; break;
				case SDLK_V: cpu.keyboard_state |= CHIP8_KEYCODE_F; break;
				}
				// clang-format on
				break;
			case SDL_EVENT_KEY_UP:
				// clang-format off
				switch (event.key.key) {
				case SDLK_1: cpu.keyboard_state &= !CHIP8_KEYCODE_1; break;
				case SDLK_2: cpu.keyboard_state &= !CHIP8_KEYCODE_2; break;
				case SDLK_3: cpu.keyboard_state &= !CHIP8_KEYCODE_3; break;
				case SDLK_4: cpu.keyboard_state &= !CHIP8_KEYCODE_C; break;
				case SDLK_Q: cpu.keyboard_state &= !CHIP8_KEYCODE_4; break;
				case SDLK_W: cpu.keyboard_state &= !CHIP8_KEYCODE_5; break;
				case SDLK_E: cpu.keyboard_state &= !CHIP8_KEYCODE_6; break;
				case SDLK_R: cpu.keyboard_state &= !CHIP8_KEYCODE_D; break;
				case SDLK_A: cpu.keyboard_state &= !CHIP8_KEYCODE_7; break;
				case SDLK_S: cpu.keyboard_state &= !CHIP8_KEYCODE_8; break;
				case SDLK_D: cpu.keyboard_state &= !CHIP8_KEYCODE_9; break;
				case SDLK_F: cpu.keyboard_state &= !CHIP8_KEYCODE_E; break;
				case SDLK_Z: cpu.keyboard_state &= !CHIP8_KEYCODE_A; break;
				case SDLK_X: cpu.keyboard_state &= !CHIP8_KEYCODE_0; break;
				case SDLK_C: cpu.keyboard_state &= !CHIP8_KEYCODE_B; break;
				case SDLK_V: cpu.keyboard_state &= !CHIP8_KEYCODE_F; break;
				}
				// clang-format on
				break;
			}
		}

		// Run every 500hz
		if (SDL_GetTicks() - last_cpu_frame >= CPU_FRAME_TIME) {
			cpu_step(&cpu);
			last_cpu_frame += DISPLAY_FRAME_TIME;
		}

		// Run every 60hz
		if (SDL_GetTicks() - last_display_frame >= DISPLAY_FRAME_TIME) {
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
			last_display_frame += DISPLAY_FRAME_TIME;
		}

		if (stream && cpu.st > 0) {
			if (SDL_AudioStreamDevicePaused(stream)) {
				SDL_ResumeAudioStreamDevice(stream);
			}
			if (SDL_GetAudioStreamQueued(stream) < 2048) {
				float sample_buffer[512];
				for (int i = 0; i < 512; i++) {
					// Generate a standard chiptune square wave profile
					float period = 44100.0f / audio_frequency;
					if (((int)(audio_phase / (period / 2.0f)) % 2) == 0) {
						sample_buffer[i] = audio_volume;
					} else {
						sample_buffer[i] = -audio_volume;
					}
					audio_phase++;
				}
				SDL_PutAudioStreamData(stream, sample_buffer, sizeof(sample_buffer));
			}
		} else {
			if (!SDL_AudioStreamDevicePaused(stream)) {
				SDL_PauseAudioStreamDevice(stream);
			}
		}

		// Slow down the processing for a bit
		SDL_Delay(1);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyAudioStream(stream);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
