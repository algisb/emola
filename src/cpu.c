#include "cpu.h"
#include <stdlib.h>
#include <stdio.h>
#include "regs.h"
#include "defs.h"
struct CPU
{
    Regs regs;
};

#define SHOW_DEBUG_INFO

#ifdef SHOW_DEBUG_INFO
#define PRINT_INSTRUCTION(_str) \
printf(_str)
#else
#define PRINT_INSTRUCTION(_str)
#endif


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
    
    if(_cpu->regs.PC > RAM_SIZE)
    {
        _cpu->regs.PC = 0;
    }
    return 0;
}

int feDeExInst(CPU * _cpu, unsigned char * _memory)
{
    Regs * regs = &_cpu->regs;
    fetchInst(_cpu, _memory);
    if(_cpu->regs.IR == 0xCB)//prefix byte, means using different instructions
    {
        _cpu->regs.PC += 1;
        fetchInst(_cpu, _memory);//fetch another instruction as prefix byte was present
        switch(_cpu->regs.IR)
        {
            
        }
    }
    else
    {
        switch(_cpu->regs.IR)
        {
            case 0x00:
            {
                PRINT_INSTRUCTION("NOP\n");
                _cpu->regs.PC += 1;
                break;
            }
            case 0x76:
            {
                PRINT_INSTRUCTION("HALT\n");
                _cpu->regs.PC += 1;
                
                printf("reg B data: %d\n", regs->B);
                printf("reg H data: %d\n", regs->H);
                while(1)//wait for interrupt or reset
                {
                    
                }

            }
            
            //8 bit loads---------------------------------------------------------------------------------
            case 0x06:
            {
                PRINT_INSTRUCTION("LD B *\n");
                regs->B = _memory[regs->PC + 1];//access the immediate memory going after the instruction
                _cpu->regs.PC += 2;
                break;
            }
            case 0x0E:
            {
                PRINT_INSTRUCTION("LD C *\n");
                regs->C = _memory[regs->PC + 1];//access the immediate memory going after the instruction
                _cpu->regs.PC += 2;
                break;
            }
            case 0x16:
            {
                PRINT_INSTRUCTION("LD D *\n");
                regs->D = _memory[regs->PC + 1];//access the immediate memory going after the instruction
                _cpu->regs.PC += 2;
                break;
            }
            case 0x1E:
            {
                PRINT_INSTRUCTION("LD E *\n");
                regs->E = _memory[regs->PC + 1];//access the immediate memory going after the instruction
                _cpu->regs.PC += 2;
                break;
            }
            case 0x26:
            {
                PRINT_INSTRUCTION("LD H *\n");
                regs->H = _memory[regs->PC + 1];//access the immediate memory going after the instruction
                _cpu->regs.PC += 2;
                break;
            }
            case 0x2E:
            {
                PRINT_INSTRUCTION("LD L *\n");
                regs->L = _memory[regs->PC + 1];//access the immediate memory going after the instruction
                _cpu->regs.PC += 2;
                break;
            }
            
            default:
                printf("Uninplemented instruction: %d\n", _cpu->regs.IR);
                
        }
    }
    return 0;
}



