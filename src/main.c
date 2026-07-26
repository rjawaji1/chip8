#include "cpu.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define FRAME_TIME 16

int main(int argc, char **argv) {
	// Initialize the cpu. This should be small enough to fit on the heap
	Cpu cpu = cpu_new();

	// Load ROM
	FILE *fptr = fopen(argv[1], "r");
	if (fptr == NULL) {
		printf("File does not exist");
		return 1;
	}

	int value;
	for (int i = 0x200; (value = fgetc(fptr)) != EOF; ++i) {
		cpu.memory[i] = value;
	}

	for (int i = 0; i < 256; ++i) {
		cpu_step(&cpu);
		usleep(FRAME_TIME);
	}
	cpu_draw_screen(&cpu);

	return 0;
}
