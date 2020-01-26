#include "cpu.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "regs.h"
#include "defs.h"
struct CPU
{
    Regs regs;
    uint32_t cycles;
    
    //disassembly tables
    void * rtable[8];
    uint16_t * rptable[4];
    uint16_t * rp2table[4];
};



struct Opcode
{
        uint8_t data;
        
        uint8_t x : 2;
        uint8_t y : 3;
        uint8_t p : 2;
        uint8_t q : 1;
        uint8_t z : 3;
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

const uint8_t xMask = 0b11000000;
const uint8_t yMask = 0b00111000;
const uint8_t pMask = 0b00110000;
const uint8_t qMask = 0b00001000;
const uint8_t zMask = 0b00000111;

//disassembly tables



typedef struct Regs Regs;


#define SHOW_DEBUG_INFO

#ifdef SHOW_DEBUG_INFO
#define PRINT_DEBUG(_str) \
printf(_str)
#else
#define PRINT_DEBUG(_str)
#endif

int logUnhandledOp (int _line, Opcode _opcode)
{
    printf("ERROR: unhandled opcode 0x%02x -> %s -> line: %d\n", _opcode.data, __FILE__, _line);
    while(1);
}
#define LOG_ERROR_OP( opcode ) logUnhandledOp( __LINE__, opcode)

static void createDisTables(CPU* _cpu)
{
    const void * rtable[8] = {&_cpu->regs.B, &_cpu->regs.C, &_cpu->regs.D, &_cpu->regs.E,  &_cpu->regs.H ,&_cpu->regs.L , &_cpu->regs.HL, &_cpu->regs.A};
    //TODO:                                                                                                                                                                                                         ^ - this one will need specia treatment as it refers to the byte pointed by (HL)
    memcpy(_cpu->rtable,  rtable, sizeof(rtable));
    const uint16_t * rptable[4] = {&_cpu->regs.BC, &_cpu->regs.DE, &_cpu->regs.HL, &_cpu->regs.SP};
    memcpy(_cpu->rptable,  rptable, sizeof(rptable));
    memcpy(_cpu->rp2table,  rptable, sizeof(rptable));
    _cpu->rp2table[3] = &_cpu->regs.AF;
}

uint8_t * getRVal(CPU * _cpu, uint8_t * _memory, uint8_t _index)
{
    if(_index == 6)
    {
         return (uint8_t *)(&_memory[*(uint16_t*)(_cpu->rtable[_index])]);
    }
    else
    {
        return (uint8_t *)(_cpu->rtable[_index]);
    }
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

static Opcode decodeOp(uint8_t _op)
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

static void fetchByte(CPU * _cpu, uint8_t * _memory)
{
    _cpu->regs.IR = _memory[_cpu->regs.PC];
    
    if(_cpu->regs.PC > RAM_SIZE)
    {
        _cpu->regs.PC = 0;
    }
}

static uint8_t getFlag(const CPU * _cpu, Flag _flag)
{
    uint8_t mask = 0b10000000 >> _flag;
    uint8_t nShift = 7 - _flag;
    return (_cpu->regs.F & mask) >> nShift; 
}
static void setFlag(CPU * _cpu, Flag _flag)
{
    uint8_t mask = 0b10000000 >> _flag;
    _cpu->regs.F  |= mask; 
}

static void resetFlag(CPU * _cpu, Flag _flag)
{
    uint8_t mask = 0b10000000 >> _flag;
    _cpu->regs.F  &= ~(mask); 
}

static uint8_t getCC(const CPU * _cpu, CC _cc)
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

void feDeExInst(CPU * _cpu, uint8_t * _memory)
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
            case 0://x
            {
                switch(opcode.z)
                {
                    case 0://z
                    {
                        switch(opcode.y)
                        {
                            case 0:
                            {
                                PRINT_DEBUG("NOP\n");
                                //TEST DUMMY VALUES
                                _cpu->regs.SP = 911;
                                _cpu->regs.F = 0b00000000;//first bit controls the relative jump
                                
                                _cpu->regs.A = 123;
                                _cpu->regs.BC = 1073;
                                _cpu->regs.D = 35;
                                uint8_t * loc0 = (uint8_t *)(&_memory[_cpu->regs.BC]);
                                *loc0 = 59;
                                
                                //TEST DUMMY VALUES
                                
                                
                                
                                _cpu->cycles += 4;
                                _cpu->regs.PC += 1;
                                break;
                            }
                            case 1:
                            {
                                PRINT_DEBUG("LD (nn) SP\n");
                                uint16_t * loc0 = (uint16_t *)(&_memory[_cpu->regs.PC + 1]);//pointer to the immediate data after the instruction
                                uint16_t * loc1 = (uint16_t *)(&_memory[*loc0]);//immediate data is a memory location, so we need to dereference
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
                                int8_t * loc0 = (int8_t *)(&_memory[_cpu->regs.PC + 1]);
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
                                    int8_t * loc0 = (int8_t *)(&_memory[_cpu->regs.PC + 1]);
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
                                LOG_ERROR_OP(opcode);
                                break;
                            }
                        }
                        break;
                    }
                    
                    case 1://z
                    {
                        switch(opcode.q)
                        {
                            case 0:
                            {
                                PRINT_DEBUG("LD rp[p] nn\n");
                                uint16_t * loc = (uint16_t *)(&_memory[_cpu->regs.PC + 1]);
                                *(_cpu->rptable[opcode.p]) = *loc;
                                
                                _cpu->regs.PC += 3;
                                _cpu->cycles += 12;
                                break;
                            }
                            
                            case 1:
                            {
                                PRINT_DEBUG("ADD HL rp[p]\n");
                                _cpu->regs.HL += *(_cpu->rptable[opcode.p]) ;
                                resetFlag(_cpu, F_N);
                                
                                //process half carry flag
                                ((_cpu->regs.HL & 0x000F) + (*(_cpu->rptable[opcode.p]) & 0x000F )) & 0x00F0 ? 
                                setFlag(_cpu, F_H) : resetFlag(_cpu, F_H);
                                
                                //process carry flag
                                ((_cpu->regs.HL & 0x00FF) + (*(_cpu->rptable[opcode.p]) & 0x00FF )) & 0x0F00 ?
                                setFlag(_cpu, F_C) : resetFlag(_cpu, F_C);
                                
                                _cpu->regs.PC += 1;
                                _cpu->cycles += 8;
                                break;
                            }
                            default:
                            {
                                LOG_ERROR_OP(opcode);
                                break;
                            }
                        }
                        
                        break;
                    }
                    
                    case 2: //z
                    {
                        switch(opcode.p)
                        {
                            case 0:
                            {
                                opcode.q == 0 ? PRINT_DEBUG("LD (BC), A\n") : PRINT_DEBUG("LD A, (BC)\n");
                                uint8_t * loc0 = (uint8_t *)(&_memory[_cpu->regs.BC]);//immediate data is a memory location, so we need to dereference
                                opcode.q == 0 ? (*loc0 = _cpu->regs.A) : (_cpu->regs.A = *loc0);
                                _cpu->regs.PC += 1;
                                _cpu->cycles += 8;
                                break;
                            }
                            
                            case 1:
                            {
                                opcode.q == 0 ? PRINT_DEBUG("LD (DE), A\n") : PRINT_DEBUG("LD A, (DE)\n");
                                uint8_t * loc0 = (uint8_t *)(&_memory[_cpu->regs.DE]);//immediate data is a memory location, so we need to dereference
                                opcode.q == 0 ? (*loc0 = _cpu->regs.A) : (_cpu->regs.A = *loc0);
                                _cpu->regs.PC += 1;
                                _cpu->cycles += 8;
                                break;
                            }
                            
                            case 2:
                            {
                                opcode.q == 0 ? PRINT_DEBUG("LD (HL+), A\n") : PRINT_DEBUG("LD A, (HL+)\n");
                                uint8_t * loc0 = (uint8_t *)(&_memory[_cpu->regs.HL]);//immediate data is a memory location, so we need to dereference
                                opcode.q == 0 ? (*loc0 = _cpu->regs.A) : (_cpu->regs.A = *loc0);
                                _cpu->regs.HL++;
                                _cpu->regs.PC += 1;
                                _cpu->cycles += 8;
                                break;
                            }
                            
                            case 3:
                            {
                                opcode.q == 0 ? PRINT_DEBUG("LD (HL-), A\n") : PRINT_DEBUG("LD A, (HL-)\n");
                                uint8_t * loc0 = (uint8_t *)(&_memory[_cpu->regs.HL]);//immediate data is a memory location, so we need to dereference
                                opcode.q == 0 ? (*loc0 = _cpu->regs.A) : (_cpu->regs.A = *loc0);
                                _cpu->regs.HL--;
                                _cpu->regs.PC += 1;
                                _cpu->cycles += 8;
                                break;
                            }
                            default:
                            {
                                LOG_ERROR_OP(opcode);
                                break;
                            }
                            
                        }
                        break;   
                    }
                    
                    
                    case 3://z
                    {
                        opcode.q == 0 ? PRINT_DEBUG("INC rp[p]\n") : PRINT_DEBUG("DEC rp[p]\n");
                        uint16_t * v  = _cpu->rptable[opcode.p];
                        opcode.q == 0 ? (*v)++ : (*v)--;
                        _cpu->regs.PC += 1;
                        _cpu->cycles += 8;
                        break;  
                    }
                    
                    case 4://z
                    case 5://z
                    {
                        opcode.z == 4 ? PRINT_DEBUG("INC r[y]\n") : PRINT_DEBUG("DEC r[y]\n");
                        uint8_t * v = getRVal(_cpu, _memory, opcode.y);
                        
                        //process flags
                        *v == 0 ? setFlag(_cpu, F_Z) : resetFlag(_cpu, F_Z);
                        opcode.z == 4 ? resetFlag(_cpu, F_N) : setFlag(_cpu, F_N);
                        //process half carry flag
                        ((*v & 0x000F) + 1) & 0x00F0 ? 
                        setFlag(_cpu, F_H) : resetFlag(_cpu, F_H);
                        
                        //do the op
                        opcode.z == 4 ? (*v)++ : (*v)--;
                        
                        _cpu->regs.PC += 1;
                        opcode.y == 6 ? (_cpu->cycles += 12) : (_cpu->cycles += 4);
                        break;
                    }
                    
                    default:
                    {
                        LOG_ERROR_OP(opcode);
                        break;
                    }
                }
                break;
            }
            
            
            case 1: //x
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
                                
                                
                                uint16_t * loc0 = (uint16_t *)&_memory[69];
                                printf("data in mem loc 69: %d\n", *loc0);
                                printf("data in HL: %d\n", regs->HL);
                                uint8_t * loc1 = (uint8_t *)&_memory[799];
                                printf("data in loc 799 %d\n", *loc1);
                                printf("data in D %d\n", _cpu->regs.D);
                                printf("dat in BC %d\n",  _cpu->regs.BC);
                                
                                
                                _cpu->cycles += 4;
                                _cpu->regs.PC += 1;
                                while(1);
                                break;
                            }
                            default:
                            {
                                LOG_ERROR_OP(opcode);
                                break;
                            }
                        }
                        break;
                    }
                    default:
                    {
                        LOG_ERROR_OP(opcode);
                        break;
                    }
                }
                break;
            }
            
            
            case 2://x
            {
                break;
            }
            case 3://x
            {
                break;
            }
            
            default:
            {
                LOG_ERROR_OP(opcode);
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



