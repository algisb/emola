#ifndef CPU_H_
#define CPU_H_
#include <stdint.h>

struct CPU;
typedef struct CPU CPU;

struct Opcode;
typedef struct Opcode Opcode;

void createCPU(CPU ** _cpu);
void destroyCPU(CPU ** _cpu);
void feDeExInst(CPU * _cpu, uint8_t * _memory);
void runTestsCPU();

#endif
