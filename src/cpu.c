#include "cpu.h"
#include <stdlib.h>
#include <stdio.h>
#include "regs.h"
#include "defs.h"
struct CPU
{
    Regs regs;
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

int fetchInst(CPU ** _cpu, unsigned char ** _ram)
{
    
    (*_cpu)->regs.IR = (*_ram)[(*_cpu)->regs.PC];
    (*_cpu)->regs.PC++;
    
    if((*_cpu)->regs.PC > RAM_SIZE)
    {
        (*_cpu)->regs.PC = 0;
    }
    
    return 0;
}

int decodeExecInst(CPU ** _cpu, unsigned char ** _ram)
{
    switch((*_cpu)->regs.IR)
    {
        case 0x0000:
            break;
        case 0x0001:
            break;
        case 0x0002:
            break;
        case 0x0003:
            break;
        default:
            printf("Uninplemented instruction: %d\n", (*_cpu)->regs.IR);
            
    }
    return 0;
}



