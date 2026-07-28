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

	cpu->pc += 2;

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
}

static inline void op_0(Cpu *cpu, uint16_t opcode) {
	switch (opcode) {
	case 0x00E0: // CLS - Clear the display
		memset(cpu->vram, 0, sizeof(cpu->vram));
		break;

	case 0x00EE: // RET - Return from subroutine
		cpu->pc = cpu->stack[cpu->sp];
		cpu->sp--;
		break;
	}
}

/*
 * 1nnn - JP addr
 * Jump to location nnn
 */
static inline void op_1(Cpu *cpu, uint16_t opcode) {
	cpu->pc = opcode & 0x0FFF;
}

/*
 * 2nnn - CALL addr
 * Call subroutine at nnn
 */
static inline void op_2(Cpu *cpu, uint16_t opcode) {
	uint16_t nnn = opcode & 0x0FFF;

	cpu->sp++;
	cpu->stack[cpu->sp] = nnn;
	cpu->pc = nnn;
}

/*
 * 3xkk - SE Vx, byte
 * Skip next instruction if Vx = kk.
 */
static inline void op_3(Cpu *cpu, uint16_t opcode) {
	uint8_t x = (opcode & 0x0F00) >> 8;
	uint8_t kk = opcode & 0x00FF;
	if (cpu->v[x] != kk)
		cpu->pc += 2;
}

/*
 * 4xkk - SNE Vx, byte
 * Skip next instruction if Vx != kk.
 */
static inline void op_4(Cpu *cpu, uint16_t opcode) {
	uint8_t x = (opcode & 0x0F00) >> 8;
	uint8_t kk = opcode & 0x00FF;
	if (cpu->v[x] == kk)
		cpu->pc += 2;
}

/*
 * 5xy0 - SE Vx, Vy
 * Skip next instruction if Vx = Vy
 */
static inline void op_5(Cpu *cpu, uint16_t opcode) {
	uint8_t x = (opcode & 0x0F00) >> 8;
	uint8_t y = (opcode & 0x00F0) >> 4;
	if (cpu->v[x] == cpu->v[y])
		cpu->pc += 2;
}

/*
 * 6xkk - LD Vx, byte
 * Set Vx = kk
 */
static inline void op_6(Cpu *cpu, uint16_t opcode) {
	uint8_t x = (opcode & 0x0F00) >> 8;
	uint8_t kk = opcode & 0x00FF;
	cpu->v[x] = kk;
}

/*
 * 7xkk - ADD Vx, byte
 * Set Vx = Vx + kk
 */
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
	// LD Vx, Vy - Stores the value of register Vy in register Vx
	case 0x0:
		cpu->v[x] = cpu->v[y];
		break;

	// OR vx, vy - Set Vx = Vx or Vy
	case 0x1:
		cpu->v[x] |= cpu->v[y];
		break;

	// AND vx, vy - Set Vx = Vx and Vy
	case 0x2:
		cpu->v[x] &= cpu->v[y];
		break;

	// XOR Vx, Vy - Set Vx = Vx XOR Vy.
	case 0x3:
		cpu->v[x] ^= cpu->v[y];
		break;

	// ADD Vx, Vy - Set Vx = Vx + Vy, set VF = carry.
	case 0x4:
		cpu->v[0xf] = ckd_add(&cpu->v[x], cpu->v[x], cpu->v[y]);
		break;

	// SUB Vx, Vy - Set Vx = Vx - Vy, set VF = NOT borrow.
	case 0x5:
		cpu->v[0xF] = cpu->v[x] > cpu->v[y];
		cpu->v[x] -= cpu->v[y];
		break;

	// SHR Vx {, Vy} - Set Vx = Vx SHR 1
	case 0x6:
		cpu->v[0xF] = (cpu->v[x] & 0b00000001) == 1;
		cpu->v[x] >>= 1;
		break;

	// SUBN Vx, Vy - Set Vx = Vy - Vx, set VF = NOT borrow.
	case 0x7:
		cpu->v[0xF] = cpu->v[y] > cpu->v[x];
		cpu->v[x] = cpu->v[y] - cpu->v[x];
		break;

	// SHL Vx {, Vy} - Set Vx = Vx SHL 1.
	case 0xE:
		cpu->v[0xF] = (cpu->v[x] & 0b10000000) == 0b10000000;
		cpu->v[x] <<= 1;
		break;
	}
}

/*
 * 9xy0 - SNE Vx, Vy
 * Skip next instruction if Vx != Vy.
 */
static inline void op_9(Cpu *cpu, uint16_t opcode) {
	uint8_t x = (opcode & 0x0F00) >> 8;
	uint8_t y = (opcode & 0x00F0) >> 4;
	if (cpu->v[x] != cpu->v[y])
		cpu->pc += 2;
}

/*
 * Annn - LD I, addr
 * Set I = nnn.
 */
static inline void op_a(Cpu *cpu, uint16_t opcode) {
	cpu->i = opcode & 0x0FFF;
}

/**
 * Bnnn - JP V0, addr
 * Jump to location nnn + V0.
 */
static inline void op_b(Cpu *cpu, uint16_t opcode) {
	cpu->pc = cpu->i + cpu->v[0];
}

/**
 * Cxkk - RND Vx, byte
 * Set Vx = random byte AND kk.
 */
static inline void op_c(Cpu *cpu, uint16_t opcode) {
	uint8_t x = (opcode & 0x0F00) >> 8;
	uint8_t kk = opcode & 0x00FF;
	cpu->v[x] = (uint8_t)rand() & kk;
}

/**
 * Dxyn - DRW Vx, Vy, nibble
 * Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
 */
static inline void op_d(Cpu *cpu, uint16_t opcode) {
	uint8_t x_pos = cpu->v[(opcode & 0x0F00) >> 8];
	uint8_t y_pos = cpu->v[(opcode & 0x00F0) >> 4];
	uint8_t n = opcode & 0x000F;

	// Clear Collision Register
	cpu->v[0xF] = 0;

	uint8_t row, col, sprite, left, right, byte, bit = 0;
	for (int i = 0; i < n; ++i) {
		sprite = cpu->memory[cpu->i + i];

		byte = x_pos >> 3; // Divide by 8
		bit = x_pos & 7;   // Get the modulo of 8 which is the last 3 bits

		row = (y_pos + i) % 32;

		// The bit is aligned
		if (bit == 0) {
			uint8_t old = cpu->vram[row * 8 + byte];
			cpu->vram[row * 8 + byte] ^= sprite;

			cpu->v[0xF] |= (old & sprite) != 0;
		} else {
			uint8_t left = sprite >> bit;
			uint8_t right = sprite << (8 - bit);

			uint8_t left_old = cpu->vram[row * 8 + byte];
			uint8_t right_old = cpu->vram[row * 8 + ((byte + 1) & 7)];

			cpu->v[0xF] |= (left_old & left) != 0;
			cpu->v[0xF] |= (right_old & right) != 0;

			cpu->vram[row * 8 + byte] ^= left;
			cpu->vram[row * 8 + ((byte + 1) & 7)] ^= right;
		}
	}
}

static inline void op_e(Cpu *cpu, uint16_t opcode) {
	return;
}

static inline void op_f(Cpu *cpu, uint16_t opcode) {
	return;
}

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
