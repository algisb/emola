#ifndef CPU_H_
#define CPU_H_
#include "regs.h"
struct CPU;
typedef struct CPU CPU;

struct Opcode;
typedef struct Opcode Opcode;

int createCPU(CPU ** _cpu);
int destroyCPU(CPU ** _cpu);
int feDeExInst(CPU * _cpu, unsigned char * _memory);
int createDisTables(CPU * _cpu);

#endif
