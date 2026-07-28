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
	 * q w e r -> 4 5 6 D
	 * a s d f -> 7 8 9 E
	 * z x c v -> A 0 B F
	 *
	 * Binary Representation On The Flags
	 * ┌──┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐
	 * │0b│0│0│0│0│0│0│0│0│0│0│0│0│0│0│0│0│
	 * │  │1│2│3│4│q│w│e│r│a│s│d│f│z│x│c│v│
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
