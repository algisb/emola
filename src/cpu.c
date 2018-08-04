#include "cpu.h"
#include <stdlib.h>
#include <stdio.h>
#include "regs.h"
struct CPU
{
    Regs regs;
    unsigned char PC;
    unsigned char Flags;
};

int createCPU(CPU ** _cpu)
{
    *_cpu = (CPU*)malloc(sizeof(CPU));
    return 0;
}

int destroyCPU(CPU ** _cpu)
{
    free(*_cpu);
    return 0;
}

