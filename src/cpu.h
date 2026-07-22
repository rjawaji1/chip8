#ifndef CPU_H
#define CPU_H

#include <stdint.h>


typedef struct {
	uint8_t memory[4096];
	uint8_t vram[256];     // 64 x 32 bits
	uint16_t pc;

	uint8_t v[16];
	uint16_t i;

	uint16_t stack[16];
	uint8_t sp;

	uint8_t dt;
	uint8_t st;
} Cpu;


Cpu cpu_new();

void cpu_step(Cpu *cpu);

// Debug
void cpu_print_registers(Cpu *cpu);

typedef void (*OpcodeHandler)(Cpu *cpu, uint16_t opcode);

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

#endif
