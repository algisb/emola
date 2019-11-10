#include "cpu.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "regs.h"
#include "defs.h"
struct CPU
{
    Regs regs;
    unsigned int cycles;
    
    //disassembly tables
    const unsigned char * rtable[8];
    const unsigned short * rptable[4];
    const unsigned short * rp2table[4];
};



struct Opcode
{
        unsigned char data;
        
        unsigned char x : 2;
        unsigned char y : 3;
        unsigned char p : 2;
        unsigned char q : 1;
        unsigned char z : 3;
};

const unsigned char xMask = 0b11000000;
const unsigned char yMask = 0b00111000;
const unsigned char pMask = 0b00110000;
const unsigned char qMask = 0b00001000;
const unsigned char zMask = 0b00000111;

//disassembly tables



typedef struct Regs Regs;


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
    createDisTables(*_cpu);
    return 0;
}

int destroyCPU(CPU ** _cpu)
{
    free(*_cpu);
    return 0;
}

static Opcode decodeOp(unsigned char _op)
{
    Opcode opcode;
    opcode.data = _op;
    //NOTE: needs to use the disassembly info in: https://gb-archive.github.io/salvage/decoding_gbz80_opcodes/Decoding%20Gamboy%20Z80%20Opcodes.html  to decode the opcode
    opcode.x = (_op & xMask) >> 6;
    opcode.y = (_op & yMask) >> 3;
    opcode.p = (_op & pMask) >> 4;
    opcode.q = (_op & qMask) >> 3;
    opcode.z = (_op & zMask);
    
    return opcode;
}

static int fetchByte(CPU * _cpu, unsigned char * _memory)
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
    fetchByte(_cpu, _memory);
    if(_cpu->regs.IR == 0xCB)//prefix byte, means using different instructions
    {
        _cpu->regs.PC += 1;
        fetchByte(_cpu, _memory);//fetch another instruction as prefix byte was present
        switch(_cpu->regs.IR)
        {
            
        }
    }
    else
    {
        Opcode opcode = decodeOp(0b11000000/*_cpu->regs.IR*/);
        
        
        printf("data: %d\n", opcode.data);
        printf("x: %d\n", opcode.x);
        printf("y: %d\n", opcode.y);
        printf("z: %d\n", opcode.z);
        printf("p: %d\n", opcode.p);
        printf("q: %d\n", opcode.q);
        

            //8 bit loads---------------------------------------------------------------------------------
//             case 0x3E:
//             {
//                 PRINT_INSTRUCTION("LD A *\n");
//                 regs->A = _memory[regs->PC + 1];//access the immediate memory going after the instruction
//                 _cpu->regs.PC += 2;
//                 _cpu->cycles += 8;
//                 break;
//             }
    
            
            //16 bit loads---------------------------------------------------------------------------------
//             case 0x01:
//             {
//                 PRINT_INSTRUCTION("LD BC **\n");
//                 _cpu->regs.L = _memory[regs->PC + 1];
//                 _cpu->regs.D = _memory[regs->PC + 2];
//                 
//                 _cpu->regs.PC += 3;
//                 _cpu->cycles += 12;
//                 break;
//             }
        
            while(1);//remove to continue execution
                //printf("Uninplemented instruction: 0x%02x\n", _cpu->regs.IR);
                
    }
    return 0;
}

int createDisTables(CPU* _cpu)
{
    //TODO:
    _cpu->rtable[0] = 0;
    //_cpu->rtable = {0,0,0,0,0,0,0,0};
    return 0;
}



