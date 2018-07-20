#ifndef CPU_H_
#define CPU_H_
#include "regs.h"
struct CPU;
typedef struct CPU CPU;

int createCPU(CPU * _cpu);
int destroyCPU(CPU * _cpu);

#endif
