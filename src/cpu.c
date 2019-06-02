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

int fetchInst(CPU * _cpu, unsigned char * _memory)
{
    _cpu->regs.IR = _memory[_cpu->regs.PC];
    //_cpu->regs.PC++;
    
    if(_cpu->regs.PC > RAM_SIZE)
    {
        _cpu->regs.PC = 0;
    }
    return 0;
}

int decodeExecInst(CPU * _cpu, unsigned char * _memory)
{
    switch(_cpu->regs.IR)
    {
        case 0x00:
            printf("NOP\n");
             _cpu->regs.PC++;
            break;
        case 0x01:
            printf("LD BC **\n");
            _cpu->regs.B = _memory[_cpu->regs.PC+1];
            _cpu->regs.C = _memory[_cpu->regs.PC+2];
            _cpu->regs.PC = _cpu->regs.PC + 3;
            break;
            
        case 0x02:
            break;
            
        case 0x03:
            break;
        default:
            printf("Uninplemented instruction: %d\n", _cpu->regs.IR);
            
    }
    return 0;
}



