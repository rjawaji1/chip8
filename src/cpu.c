#include <assert.h>
#include <stdckdint.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cpu.h"

Cpu cpu_new() {
	return (Cpu){
		.memory =
			{
				0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
				0x20, 0x60, 0x20, 0x20, 0x70, // 1
				0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
				0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
				0x90, 0x90, 0xF0, 0x10, 0x10, // 4
				0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
				0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
				0xF0, 0x10, 0x20, 0x40, 0x40, // 7
				0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
				0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
				0xF0, 0x90, 0xF0, 0x90, 0x90, // A
				0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
				0xF0, 0x80, 0x80, 0x80, 0xF0, // C
				0xE0, 0x90, 0x90, 0x90, 0xE0, // D
				0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
				0xF0, 0x80, 0xF0, 0x80, 0x80, // F
			},
		.pc = 0x200,
	};
}

void cpu_step(Cpu *cpu) {
	uint16_t opcode = cpu->memory[cpu->pc] << 8 | cpu->memory[cpu->pc + 1];
	printf("0x%x\n", opcode);

	// clang-format off
	switch ((opcode & 0xF000) >> 12) {
	case 0x0: op_0(cpu, opcode); break;
	case 0x1: op_1(cpu, opcode); break;
	case 0x2: op_2(cpu, opcode); break;
	case 0x3: op_3(cpu, opcode); break;
	case 0x4: op_4(cpu, opcode); break;
	case 0x5: op_5(cpu, opcode); break;
	case 0x6: op_6(cpu, opcode); break;
	case 0x7: op_7(cpu, opcode); break;
	case 0x8: op_8(cpu, opcode); break;
	case 0x9: op_9(cpu, opcode); break;
	case 0xA: op_a(cpu, opcode); break;
	case 0xB: op_b(cpu, opcode); break;
	case 0xC: op_c(cpu, opcode); break;
	case 0xD: op_d(cpu, opcode); break;
	case 0xE: op_e(cpu, opcode); break;
	case 0xF: op_f(cpu, opcode); break;
	}
	// clang-format on

	cpu->pc += 2;
}

static inline void op_0(Cpu *cpu, uint16_t opcode) {
	if (opcode == 0x00E0) {
		memset(cpu->vram, 0, sizeof(cpu->vram));
	} else if (opcode == 0x00EE) {
		cpu->pc = cpu->stack[cpu->sp] - 2;
		cpu->sp--;
	}
}

static inline void op_1(Cpu *cpu, uint16_t opcode) {
	cpu->sp++;
	cpu->stack[cpu->sp] = cpu->pc;
}

static inline void op_2(Cpu *cpu, uint16_t opcode) {
	uint16_t nnn = opcode & 0x0FFF;

	cpu->sp++;
	cpu->stack[cpu->sp] = nnn;
	cpu->pc = nnn - 2;
}

static inline void op_3(Cpu *cpu, uint16_t opcode) {
	uint8_t x = (opcode & 0x0F00) >> 8;
	uint8_t kk = opcode & 0x00FF;
	if (cpu->v[x] != kk)
		cpu->pc += 2;
}

static inline void op_4(Cpu *cpu, uint16_t opcode) {
	uint8_t x = (opcode & 0x0F00) >> 8;
	uint8_t kk = opcode & 0x00FF;
	if (cpu->v[x] == kk)
		cpu->pc += 2;
}

static inline void op_5(Cpu *cpu, uint16_t opcode) {
	uint8_t x = (opcode & 0x0F00) >> 8;
	uint8_t y = (opcode & 0x00F0) >> 4;
	if (cpu->v[x] == cpu->v[y])
		cpu->pc += 2;
}

static inline void op_6(Cpu *cpu, uint16_t opcode) {
	uint8_t x = (opcode & 0x0F00) >> 8;
	uint8_t kk = opcode & 0x00FF;
	cpu->v[x] = kk;
}

static inline void op_7(Cpu *cpu, uint16_t opcode) {
	uint8_t x = (opcode & 0x0F00) >> 8;
	uint8_t kk = opcode & 0x00FF;
	cpu->v[x] += kk;
}

static inline void op_8(Cpu *cpu, uint16_t opcode) {
	uint8_t x = (opcode & 0x0F00) >> 8;
	uint8_t y = (opcode & 0x00F0) >> 4;
	uint8_t n = opcode & 0x00F0;

	switch (n) {
	case 0x1:
		cpu->v[x] |= cpu->v[y];
		break;
	case 0x2:
		cpu->v[x] &= cpu->v[y];
		break;
	case 0x3:
		cpu->v[x] ^= cpu->v[y];
		break;
	case 0x4:
		cpu->v[0xf] = ckd_add(&cpu->v[x], cpu->v[x], cpu->v[y]);
		break;
	case 0x5:
		cpu->v[0xF] = cpu->v[x] > cpu->v[y];
		cpu->v[x] -= cpu->v[y];
		break;
	case 0x6:
		cpu->v[0xF] = (cpu->v[x] & 0b00000001) == 1;
		cpu->v[x] >>= 1;
		break;
	case 0x7:
		cpu->v[0xF] = cpu->v[y] > cpu->v[x];
		cpu->v[x] = cpu->v[y] - cpu->v[x];
		break;
	case 0xE:
		cpu->v[0xF] = (cpu->v[x] & 0b10000000) == 0b10000000;
		cpu->v[x] <<= 1;
		break;
	}
}

static inline void op_9(Cpu *cpu, uint16_t opcode) {
	uint8_t x = (opcode & 0x0F00) >> 8;
	uint8_t y = (opcode & 0x00F0) >> 4;
	if (cpu->v[x] != cpu->v[y])
		cpu->pc += 2;
}

static inline void op_a(Cpu *cpu, uint16_t opcode) { cpu->i = opcode & 0x0FFF; }

static inline void op_b(Cpu *cpu, uint16_t opcode) { cpu->pc = cpu->i + cpu->v[0] - 2; }

static inline void op_c(Cpu *cpu, uint16_t opcode) {
	uint8_t x = (opcode & 0x0F00) >> 8;
	uint8_t kk = opcode & 0x00FF;
	cpu->v[x] = (rand() % 256) & kk;
}

static inline void op_d(Cpu *cpu, uint16_t opcode) {
	uint8_t x_pos = cpu->v[(opcode & 0x0F00) >> 8];
	uint8_t y_pos = cpu->v[(opcode & 0x00F0) >> 4];
	uint8_t n = opcode & 0x000F;

	uint8_t row, col, sprite, left, right, byte, bit = 0;
	for (int i = 0; i < n; ++i) {
		sprite = cpu->memory[cpu->i + i];

		byte = x_pos >> 3; // Divide by 8
		bit = x_pos & 7;   // Get the remaider of 8 which is the last 3 bits

		left = sprite << (8 - bit);
		right = sprite >> bit;

		row = (y_pos + i) % 32;

		cpu->vram[row * 8 + byte] ^= left;
		cpu->vram[row * 8 + (byte + 1) % 64] ^= right;
	}
}

static inline void op_e(Cpu *cpu, uint16_t opcode) { return; }

static inline void op_f(Cpu *cpu, uint16_t opcode) { return; }

void cpu_print_registers(Cpu *cpu) {
	printf("Registers:\n");
	printf("    0: %x\n", cpu->v[0x0]);
	printf("    1: %x\n", cpu->v[0x1]);
	printf("    2: %x\n", cpu->v[0x2]);
	printf("    3: %x\n", cpu->v[0x3]);
	printf("    4: %x\n", cpu->v[0x4]);
	printf("    5: %x\n", cpu->v[0x5]);
	printf("    6: %x\n", cpu->v[0x6]);
	printf("    7: %x\n", cpu->v[0x7]);
	printf("    8: %x\n", cpu->v[0x8]);
	printf("    9: %x\n", cpu->v[0x9]);
	printf("    A: %x\n", cpu->v[0xA]);
	printf("    B: %x\n", cpu->v[0xB]);
	printf("    C: %x\n", cpu->v[0xC]);
	printf("    D: %x\n", cpu->v[0xD]);
	printf("    E: %x\n", cpu->v[0xE]);
	printf("    F: %x\n", cpu->v[0xF]);
	printf("    PC: %x\n", cpu->pc);
}

/**
 * Draws the video buffer to the termianl screen
 */
void cpu_draw_screen(Cpu *cpu) {
	// Clear the terminal
	printf("\e[1;1H\e[2J");

	// Construct the buffer, note the + 1 is for
	// the \n and \0 characters the
	char buffer[32 * (64 + 1) + 1];
	char *p = buffer;

	// Draw into the terminal
	for (int row = 0; row < 32; ++row) {
		for (int col = 0; col < 8; ++col) {
			uint8_t byte = cpu->vram[row * 8 + col];
			for (int bit = 0x80; bit != 0x00; bit >>= 1) {
				*p++ = ((byte & bit) == bit) ? '1' : ' ';
			}
		}
		*p++ = '\n';
	}
	*p = '\0';

	// Display the buffer to the screen
	fputs(buffer, stdout);
	fflush(stdout);
};
