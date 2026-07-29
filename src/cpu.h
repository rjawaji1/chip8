#ifndef CPU_H
#define CPU_H

#include <stdint.h>

typedef struct {
	uint8_t memory[4096];
	uint16_t pc;

	/*
	 * Compact keyboard state Mapped from keyboard to chip 8.
	 * In the future this will ideally be configurable
	 *
	 * 1 2 3 4 -> 1 2 3 C
	 * Q W E R -> 4 5 6 D
	 * A S D F -> 7 8 9 E
	 * Z X C V -> A 0 B F
	 *
	 * Binary Representation Of The Flags
	 * ┌──┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐
	 * │0b│0│0│0│0│0│0│0│0│0│0│0│0│0│0│0│0│
	 * │  │F│E│D│C│B│A│9│8│7│6│5│4│3│2│1│0│
	 * └──┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┘
	 */
	uint16_t keyboard_state;

	uint8_t vram[256]; // 64 x 32 bits

	uint8_t v[16];
	uint16_t i;

	uint16_t stack[16];
	uint8_t sp;

	uint8_t dt;
	uint8_t st;
} Cpu;

enum CHIP8_KEYCODE {
	CHIP8_KEYCODE_0 = 1,
	CHIP8_KEYCODE_1 = 1 << 1,
	CHIP8_KEYCODE_2 = 1 << 2,
	CHIP8_KEYCODE_3 = 1 << 3,
	CHIP8_KEYCODE_4 = 1 << 4,
	CHIP8_KEYCODE_5 = 1 << 5,
	CHIP8_KEYCODE_6 = 1 << 6,
	CHIP8_KEYCODE_7 = 1 << 7,
	CHIP8_KEYCODE_8 = 1 << 8,
	CHIP8_KEYCODE_9 = 1 << 9,
	CHIP8_KEYCODE_A = 1 << 10,
	CHIP8_KEYCODE_B = 1 << 11,
	CHIP8_KEYCODE_C = 1 << 12,
	CHIP8_KEYCODE_D = 1 << 13,
	CHIP8_KEYCODE_E = 1 << 14,
	CHIP8_KEYCODE_F = 1 << 15,
};

Cpu cpu_new();

void cpu_step(Cpu *cpu);

// Instructions
static inline void op_0(Cpu *cpu, uint16_t opcode);
static inline void op_1(Cpu *cpu, uint16_t opcode);
static inline void op_2(Cpu *cpu, uint16_t opcode);
static inline void op_3(Cpu *cpu, uint16_t opcode);
static inline void op_4(Cpu *cpu, uint16_t opcode);
static inline void op_5(Cpu *cpu, uint16_t opcode);
static inline void op_6(Cpu *cpu, uint16_t opcode);
static inline void op_7(Cpu *cpu, uint16_t opcode);
static inline void op_8(Cpu *cpu, uint16_t opcode);
static inline void op_9(Cpu *cpu, uint16_t opcode);
static inline void op_a(Cpu *cpu, uint16_t opcode);
static inline void op_b(Cpu *cpu, uint16_t opcode);
static inline void op_c(Cpu *cpu, uint16_t opcode);
static inline void op_d(Cpu *cpu, uint16_t opcode);
static inline void op_e(Cpu *cpu, uint16_t opcode);
static inline void op_f(Cpu *cpu, uint16_t opcode);

// Debug
void cpu_print_registers(Cpu *cpu);

#endif
