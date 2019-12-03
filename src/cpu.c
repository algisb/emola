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
    const void * rtable[8];
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

enum Flag
{
    F_Z,
    F_N,
    F_H,
    F_C    
};
typedef enum Flag Flag;

enum CC
{
    NZ,
    Z,
    NC,
    C    
};
typedef enum CC CC;

const unsigned char xMask = 0b11000000;
const unsigned char yMask = 0b00111000;
const unsigned char pMask = 0b00110000;
const unsigned char qMask = 0b00001000;
const unsigned char zMask = 0b00000111;

//disassembly tables



typedef struct Regs Regs;


#define SHOW_DEBUG_INFO

#ifdef SHOW_DEBUG_INFO
#define PRINT_DEBUG(_str) \
printf(_str)
#else
#define PRINT_DEBUG(_str)
#endif

static void createDisTables(CPU* _cpu)
{
    const void * rtable[8] = {&_cpu->regs.B, &_cpu->regs.C, &_cpu->regs.D, &_cpu->regs.E,  &_cpu->regs.H ,&_cpu->regs.L , &_cpu->regs.HL, &_cpu->regs.A};
    //TODO:                                                                                                                                                                                                         ^ - this one will need specia treatment as it refers to the byte pointed by (HL)
    memcpy(_cpu->rtable,  rtable, sizeof(rtable));
    const unsigned short * rptable[4] = {&_cpu->regs.BC, &_cpu->regs.DE, &_cpu->regs.HL, &_cpu->regs.SP};
    memcpy(_cpu->rptable,  rptable, sizeof(rptable));
    memcpy(_cpu->rp2table,  rptable, sizeof(rptable));
    _cpu->rp2table[3] = &_cpu->regs.AF;
}

void createCPU(CPU ** _cpu)
{
    *_cpu = (CPU*)malloc(sizeof(CPU));
    createDisTables(*_cpu);
}

void destroyCPU(CPU ** _cpu)
{
    free(*_cpu);
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

static void fetchByte(CPU * _cpu, unsigned char * _memory)
{
    _cpu->regs.IR = _memory[_cpu->regs.PC];
    
    if(_cpu->regs.PC > RAM_SIZE)
    {
        _cpu->regs.PC = 0;
    }
}

static unsigned char getFlag(const CPU * _cpu, Flag _flag)
{
    unsigned char mask = 0b10000000 >> _flag;
    unsigned char nShift = 7 - _flag;
    return (_cpu->regs.F & mask) >> nShift; 
}

static unsigned char getCC(const CPU * _cpu, CC _cc)
{
    switch(_cc)
    {
        case NZ:
            return !getFlag(_cpu, F_Z);
        case Z:
            return getFlag(_cpu, F_Z);
        case NC:
            return !getFlag(_cpu, F_C);
        case C:
            return getFlag(_cpu, F_C);
    }
}

void feDeExInst(CPU * _cpu, unsigned char * _memory)
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
        Opcode opcode = decodeOp(_cpu->regs.IR);
        
        
//         printf("data: %d\n", opcode.data);
//         printf("x: %d\n", opcode.x);
//         printf("y: %d\n", opcode.y);
//         printf("z: %d\n", opcode.z);
//         printf("p: %d\n", opcode.p);
//         printf("q: %d\n", opcode.q);
        
        switch(opcode.x)
        {
            case 0:
            {
                switch(opcode.z)
                {
                    case 0:
                    {
                        switch(opcode.y)
                        {
                            case 0:
                            {
                                PRINT_DEBUG("NOP\n");
                                //TEST DUMMY VALUES
                                _cpu->regs.SP = 911;
                                _cpu->regs.F = 0b10000000;
                                //TEST DUMMY VALUES
                                
                                
                                
                                _cpu->cycles += 4;
                                _cpu->regs.PC += 1;
                                break;
                            }
                            case 1:
                            {
                                PRINT_DEBUG("LD (nn) SP\n");
                                unsigned short * loc0 = (unsigned short *)(&_memory[_cpu->regs.PC + 1]);//pointer to the immediate data after the instruction
                                unsigned short * loc1 = (unsigned short *)(&_memory[*loc0]);//immediate data is a memory location, so we need to dereference
                                *loc1 = _cpu->regs.SP;
                                _cpu->cycles += 20;
                                _cpu->regs.PC += 3;
                                break;
                            }
                            case 2:
                            {
                                PRINT_DEBUG("STOP\n");
                                _cpu->cycles += 4;
                                _cpu->regs.PC += 2;
                                break;
                            }
                            
                            case 3:
                            {
                                PRINT_DEBUG("JN d\n");
                                char * loc0 = (char *)(&_memory[_cpu->regs.PC + 1]);
                                _cpu->regs.PC += 2;
                                _cpu->regs.PC += *loc0;
                                _cpu->cycles += 12;
                                break;
                            }

                            case 4:
                            case 5:
                            case 6:
                            case 7:
                            {
                                PRINT_DEBUG("JN cc[y-4] d\n");
                                if(getCC(_cpu, opcode.y - 4))
                                {
                                    char * loc0 = (char *)(&_memory[_cpu->regs.PC + 1]);
                                    _cpu->regs.PC += 2;
                                    _cpu->regs.PC += *loc0;
                                    _cpu->cycles += 12;
                                }
                                else
                                {
                                    _cpu->regs.PC += 2;
                                    _cpu->cycles += 8;
                                }
                                break;
                            }
                            
                            default:
                            {
                                printf("Error: unhandled opcode x: %d z: %d (y: %d)\n", opcode.x, opcode.z, opcode.y);
                                break;
                            }
                        }
                        break;
                    }
                    
                    default:
                    {
                        printf("Error: unhandled opcode x: %d (z: %d)  y: %d)\n", opcode.x, opcode.z, opcode.y);
                        break;
                    }
                }
                break;
            }
            
            
            case 1:
            {
                switch(opcode.z)
                {
                    case 6:
                    {
                        switch(opcode.y)
                        {
                            case 6:
                            {
                                PRINT_DEBUG("HALT\n");
                                
                                
                                unsigned short * loc0 = (unsigned short *)&_memory[69];
                                printf("DATA IN MEM LOC 69: %d\n", *loc0);
                                
                                
                                
                                
                                _cpu->cycles += 4;
                                _cpu->regs.PC += 1;
                                while(1);
                                break;
                            }
                            default:
                            {
                                printf("Error: unhandled opcode x: %d z: %d (y: %d)\n", opcode.x, opcode.z, opcode.y);
                                break;
                            }
                        }
                        break;
                    }
                    default:
                    {
                        printf("Error: unhandled opcode x: %d (z: %d)  y: %d)\n", opcode.x, opcode.z, opcode.y);
                        break;
                    }
                }
                break;
            }
            
            
            case 2:
            {
                break;
            }
            case 3:
            {
                break;
            }
            
            default:
            {
                printf("Error: unhandled opcode (x: %d) z: %d y: %d)\n", opcode.x, opcode.z, opcode.y);
                break;
            }
        }
        

            //8 bit loads---------------------------------------------------------------------------------
//             case 0x3E:
//             {
//                 PRINT_DEBUG("LD A *\n");
//                 regs->A = _memory[regs->PC + 1];//access the immediate memory going after the instruction
//                 _cpu->regs.PC += 2;
//                 _cpu->cycles += 8;
//                 break;
//             }
    
            
            //16 bit loads---------------------------------------------------------------------------------
//             case 0x01:
//             {
//                 PRINT_DEBUG("LD BC **\n");
//                 _cpu->regs.L = _memory[regs->PC + 1];
//                 _cpu->regs.D = _memory[regs->PC + 2];
//                 
//                 _cpu->regs.PC += 3;
//                 _cpu->cycles += 12;
//                 break;
//             }
        
//            while(1);//remove to continue execution
                //printf("Uninplemented instruction: 0x%02x\n", _cpu->regs.IR);
                
    }
}



