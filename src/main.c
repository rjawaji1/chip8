#include "cpu.h"
#include <stdint.h>

int main(int argc, char **argv){
	// Initialize the cpu. This should be small enough to fit on the heap
	Cpu cpu = cpu_new();

	// Load ROM
	uint8_t rom[4] = {
		0x1F, 0xF1,
		0x1b, 0x1b
	};

	for(int i = 0; i < 4; i++){
		cpu.memory[cpu.pc + i] = rom[i];
	}

	cpu_print_registers(&cpu);
	cpu_step(&cpu);
	cpu_print_registers(&cpu);
	cpu_step(&cpu);

	return 0;
}

