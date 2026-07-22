#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "cpu.h"

Cpu cpu_new() {
	return (Cpu){
		.memory = {
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

static OpcodeHandler handlers[16] = {
	op_0, op_1, op_2, op_3,
	op_4, op_5, op_6, op_7,
	op_8, op_9, op_a, op_b,
	op_c, op_d, op_e, op_f,
};

void cpu_step(Cpu *cpu){
	uint16_t opcode = cpu->memory[cpu->pc] << 8 | cpu->memory[(cpu->pc) + 1];
	handlers[(opcode & 0xF000) >> 12](cpu, opcode);
	cpu->pc+=2;

	uint8_t code = (opcode & 0xF000) >> 12;

	uint16_t nnn = opcode & 0x0FFF;
	uint8_t kk =  opcode & 0x00FF;
	uint8_t x = (opcode & 0x0F00) >> 8;
	uint8_t y = (opcode & 0x00F0) >> 4;
	uint8_t n = (opcode & 0x000F);


	switch (opcode & 0xF000) {
		case 0x0000:
			if(kk == 0xE0){
				memset(cpu->memory,0,sizeof(cpu->memory));
			}
			else if(kk == 0xEE){
				cpu->pc = cpu->stack[cpu->sp];
				cpu->sp--;
			}
			break;
		case 0x1000:
			cpu->pc = nnn;
			break;
		case 0x2000:
			cpu->sp++;
			cpu->stack[cpu->sp] = nnn;
			cpu->pc = nnn;
		case 0x3000:
			if(cpu->v[x] != kk) cpu->pc += 4;
			else cpu->pc += 2;
			break;
		case 0x4000:
			if(cpu->v[x] != kk) cpu -> pc += 4;
			else cpu->pc += 2;
			break;
		case 0x5000:
			if(cpu->v[x] == cpu->v[y]) cpu -> pc += 4;
			else cpu->pc += 2;
			break;
		case 0x6000:
			cpu -> v[x] = kk;
		default:
			cpu -> pc += 2;
	}
}

static void op_0(Cpu *cpu, uint16_t opcode){
	if(opcode == 0x00E0) {
		memset(cpu->vram,0,sizeof(cpu->vram));
	}
	else if (opcode == 0x00EE) {
		cpu->pc = cpu->stack[cpu->sp];
		cpu->pc--;
	}
	return;
}

static void op_1(Cpu *cpu, uint16_t opcode){
	return;
}

static void op_2(Cpu *cpu, uint16_t opcode){
	return;
}

static void op_3(Cpu *cpu, uint16_t opcode){
	return;
}

static void op_4(Cpu *cpu, uint16_t opcode){
	return;
}

static void op_5(Cpu *cpu, uint16_t opcode){
	return;
}

static void op_6(Cpu *cpu, uint16_t opcode){
	return;
}

static void op_7(Cpu *cpu, uint16_t opcode){
	return;
}

static void op_8(Cpu *cpu, uint16_t opcode){
	return;
}

static void op_9(Cpu *cpu, uint16_t opcode){
	return;
}

static void op_a(Cpu *cpu, uint16_t opcode){
	return;
}

static void op_b(Cpu *cpu, uint16_t opcode){
	return;
}

static void op_c(Cpu *cpu, uint16_t opcode){
	return;
}

static void op_d(Cpu *cpu, uint16_t opcode){
	return;
}

static void op_e(Cpu *cpu, uint16_t opcode){
	return;
}

static void op_f(Cpu *cpu, uint16_t opcode){
	return;
}

void cpu_print_registers(Cpu *cpu){
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
