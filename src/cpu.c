#include "cpu.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "cpu_t.h"
#include "alu.h"


const uint8_t xMask = 0b11000000;
const uint8_t yMask = 0b00111000;
const uint8_t pMask = 0b00110000;
const uint8_t qMask = 0b00001000;
const uint8_t zMask = 0b00000111;

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
    (*_cpu)->regs.SP = 0xFFFE;
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

uint8_t getFlag(const CPU * _cpu, Flag _flag)
{
    uint8_t mask = 0b10000000 >> _flag;
    uint8_t nShift = 7 - _flag;
    return (_cpu->regs.F & mask) >> nShift; 
}
void setFlag(CPU * _cpu, Flag _flag)
{
    uint8_t mask = 0b10000000 >> _flag;
    _cpu->regs.F  |= mask; 
}

void resetFlag(CPU * _cpu, Flag _flag)
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

void deExInst(CPU * _cpu, uint8_t * _memory, uint8_t _op)
{
            Opcode opcode = decodeOp(_op);
        
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
                            uint32_t c = ((_cpu->regs.HL & 0x00FF) + (*(_cpu->rptable[opcode.p]) & 0x00FF )) & 0x0F00; 
                            c ? setFlag(_cpu, F_C) : resetFlag(_cpu, F_C);
                            
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
                
                case 6://z
                {
                    PRINT_DEBUG("LD r[y], n\n");
                    uint8_t * loc = (uint8_t *)(&_memory[_cpu->regs.PC + 1]);
                    uint8_t * v = getRVal(_cpu, _memory, opcode.y);
                    *v = *loc;
                    
                    _cpu->regs.PC += 2;
                    opcode.y == 6 ? (_cpu->cycles += 12) : (_cpu->cycles += 8);
                    break;
                }
                case 7://z
                {
                    switch(opcode.y)
                    {
                        case 0://y
                        {
                            PRINT_DEBUG("RLCA\n");
                            uint8_t tmp = _cpu->regs.A & 0b10000000;
                            tmp = tmp >> 7;
                            _cpu->regs.A = _cpu->regs.A << 1;
                            tmp ? setFlag(_cpu, F_C) : resetFlag(_cpu, F_C);
                            _cpu->regs.A = (_cpu->regs.A & 0b11111110) | tmp;
                            resetFlag(_cpu, F_Z);
                            resetFlag(_cpu, F_N);
                            resetFlag(_cpu, F_H);
                            
                            _cpu->regs.PC += 1;
                            _cpu->cycles += 4;
                            break;
                        }
                        
                        case 1:
                        {
                            PRINT_DEBUG("RRCA\n");
                            uint8_t tmp = _cpu->regs.A & 0b00000001;
                            tmp = tmp << 7;
                            _cpu->regs.A = _cpu->regs.A >> 1;
                            tmp ? setFlag(_cpu, F_C) : resetFlag(_cpu, F_C);
                            _cpu->regs.A = (_cpu->regs.A & 0b01111111) | tmp;
                            resetFlag(_cpu, F_Z);
                            resetFlag(_cpu, F_N);
                            resetFlag(_cpu, F_H);
                            
                            _cpu->regs.PC += 1;
                            _cpu->cycles += 4;
                            
                            break;
                        }
                        
                        case 2:
                        {
                            PRINT_DEBUG("RLA\n");
                            uint8_t tmp = _cpu->regs.A & 0b10000000;
                            tmp = tmp >> 7;
                            _cpu->regs.A = _cpu->regs.A << 1;
                            _cpu->regs.A = (_cpu->regs.A & 0b11111110) | getFlag(_cpu, F_C);
                            tmp ? setFlag(_cpu, F_C) : resetFlag(_cpu, F_C);
                            resetFlag(_cpu, F_Z);
                            resetFlag(_cpu, F_N);
                            resetFlag(_cpu, F_H);
                            
                            _cpu->regs.PC += 1;
                            _cpu->cycles += 4;
                            
                            break;
                        }
                        case 3:
                        {
                            PRINT_DEBUG("RRA\n");
                            uint8_t tmp = _cpu->regs.A & 0b00000001;
                            tmp = tmp << 7;
                            _cpu->regs.A = _cpu->regs.A >> 1;
                            _cpu->regs.A = (_cpu->regs.A & 0b01111111) | getFlag(_cpu, F_C);
                            tmp ? setFlag(_cpu, F_C) : resetFlag(_cpu, F_C);
                            resetFlag(_cpu, F_Z);
                            resetFlag(_cpu, F_N);
                            resetFlag(_cpu, F_H);
                            
                            _cpu->regs.PC += 1;
                            _cpu->cycles += 4;
                            break;
                        }
                        
                        case 4:
                        {
                            PRINT_DEBUG("DAA\n");
                            int reset = 0;
                            if (!getFlag(_cpu, F_N))
                            {
                                if ((_cpu->regs.A & 0x0F) > 9 || getFlag(_cpu, F_H))
                                {
                                    //add 0x06
                                    _cpu->regs.A += 0x06;
                                }
                                if (((_cpu->regs.A & 0xF0) >> 4)  > 9 || getFlag(_cpu, F_C) ) 
                                {
                                    reset = 1;
                                    _cpu->regs.A += 0x60;
                                }
                            }
                            else
                            {
                                if (getFlag(_cpu, F_H))
                                {
                                    _cpu->regs.A -= 0x06;
                                }
                                if ( getFlag(_cpu, F_C) ) 
                                {
                                    _cpu->regs.A -= 0x60;
                                }
                                
                            }
                            resetFlag(_cpu, F_H);
                            if(reset)
                                resetFlag(_cpu, F_C);
                            
                            _cpu->regs.A == 0 ? setFlag(_cpu, F_Z ) : resetFlag(_cpu, F_Z ) ;
                            
                            _cpu->regs.PC += 1;
                            _cpu->cycles += 4;
                            break;
                        }
                        
                        case  5:
                        {
                            PRINT_DEBUG("CPL\n");
                            _cpu->regs.A = ~(_cpu->regs.A);
                            
                            setFlag(_cpu, F_N);
                            setFlag(_cpu, F_H);
                            
                            _cpu->regs.PC += 1;
                            _cpu->cycles += 4;
                            break;
                        }
                        
                        case 6:
                        {
                            PRINT_DEBUG("SCF\n");
                            setFlag(_cpu, F_C);
                            resetFlag(_cpu, F_N);
                            resetFlag(_cpu, F_H);
                            
                            _cpu->regs.PC += 1;
                            _cpu->cycles += 4;
                            break;
                        }
                        
                        case 7:
                        {
                            PRINT_DEBUG("CCF\n");
                            
                            if(getFlag(_cpu, F_C))
                                resetFlag(_cpu, F_C);
                            else
                                setFlag(_cpu, F_C);
                            
                            resetFlag(_cpu, F_N);
                            resetFlag(_cpu, F_H);
                            
                            _cpu->regs.PC += 1;
                            _cpu->cycles += 4;
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
        
        
        case 1: //x
        {
            if (opcode.z == 6 || opcode.y == 6)
            {
                PRINT_DEBUG("HALT\n");
                uint16_t * loc0 = (uint16_t *)&_memory[69];
                printf("data in mem loc 69: %d\n", *loc0);
                printf("data in HL: %d\n", _cpu->regs.HL);
                uint8_t * loc1 = (uint8_t *)&_memory[799];
                printf("data in loc 799 %d\n", *loc1);
                printf("data in D %d\n", _cpu->regs.D);
                printf("dat in BC %d\n",  _cpu->regs.BC);
                printf("dat in B %d\n",  _cpu->regs.B);
                
                _cpu->cycles += 4;
                _cpu->regs.PC += 1;
                while(1);
            }
            else
            {
                PRINT_DEBUG("LD r[y] r[z]\n");
                uint8_t * v0 = getRVal(_cpu, _memory, opcode.y);
                uint8_t * v1 = getRVal(_cpu, _memory, opcode.z);
                *v0 = *v1;
                
                opcode.y == 6 || opcode.z == 6 ? (_cpu->cycles += 8) : ( _cpu->cycles += 4);
                _cpu->regs.PC += 1;
            }
            
            break;
        }
        
        
        case 2://x
        {
            //ALU
            switch(opcode.y)
            {
                case 0:
                {
                    PRINT_DEBUG("ADD\n");
                    add(_cpu, getRVal(_cpu, _memory,  opcode.z));
                    
                    _cpu->cycles += opcode.z == 6 ? 8 : 4;
                    _cpu->regs.PC += 1;
                    break;
                }
                case 1:
                {
                    PRINT_DEBUG("ADC\n");
                    adc(_cpu, getRVal(_cpu, _memory,  opcode.z));
                    
                    _cpu->cycles += opcode.z == 6 ? 8 : 4;
                    _cpu->regs.PC += 1;
                    break;
                }   
                case 2:
                {
                    PRINT_DEBUG("SUB\n");
                    sub(_cpu, getRVal(_cpu, _memory,  opcode.z));
                    
                    _cpu->cycles += opcode.z == 6 ? 8 : 4;
                    _cpu->regs.PC += 1;
                    break;
                }
                case 3:
                {
                    PRINT_DEBUG("SBC\n");
                    sbc(_cpu, getRVal(_cpu, _memory,  opcode.z));
                    
                    _cpu->cycles += opcode.z == 6 ? 8 : 4;
                    _cpu->regs.PC += 1;
                    break;
                }
                case 4:
                {
                    PRINT_DEBUG("AND\n");
                    and(_cpu, getRVal(_cpu, _memory,  opcode.z));
                    
                    _cpu->cycles += opcode.z == 6 ? 8 : 4;
                    _cpu->regs.PC += 1;
                    break;
                }
                case 5:
                {
                    PRINT_DEBUG("XOR\n");
                    xor(_cpu, getRVal(_cpu, _memory,  opcode.z));
                    
                    _cpu->cycles += opcode.z == 6 ? 8 : 4;
                    _cpu->regs.PC += 1;
                    break;
                }
                case 6:
                {
                    PRINT_DEBUG("or\n");
                    or(_cpu, getRVal(_cpu, _memory,  opcode.z));
                    
                    _cpu->cycles += opcode.z == 6 ? 8 : 4;
                    _cpu->regs.PC += 1;
                    break;
                }
                case 7:
                {
                    PRINT_DEBUG("AND\n");
                    cp(_cpu, getRVal(_cpu, _memory,  opcode.z));
                    
                    _cpu->cycles += opcode.z == 6 ? 8 : 4;
                    _cpu->regs.PC += 1;
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

}

void deExInstPrefixed(CPU * _cpu, uint8_t * _memory, uint8_t _op)
{
        
}

void feDeExInst(CPU * _cpu, uint8_t * _memory)
{
    fetchByte(_cpu, _memory);
    if(_cpu->regs.IR == 0xCB)//prefix byte, means using different instructions
    {
        _cpu->regs.PC += 1;
        fetchByte(_cpu, _memory);//fetch another instruction as prefix byte was present
        
        deExInstPrefixed(_cpu, _memory, _cpu->regs.IR);
    }
    else
    {
        deExInst(_cpu, _memory, _cpu->regs.IR);
    }
}

int testEval(char * _testName, int _testCondition)
{
    if(_testCondition)
    {
        printf("test %s - PASS\n", _testName);
        return 1;
    }
    else
    {
        printf("test %s - FAIL\n", _testName);
        return 0;
    }
}

void runTestsCPU()
{
    CPU * tmpCpu;
    createCPU(&tmpCpu);
    uint8_t * tmpRam = (uint8_t*)calloc(10, 1);
    
    int pass = 1;
    //BEGIN test
    {
        //test RLCA
        tmpCpu->regs.A = 0b10010100;
        deExInst(tmpCpu, tmpRam, 0x07);
        if(!testEval("RLCA", tmpCpu->regs.A == 0b00101001))
            pass = 0;
        //test RRCA
        tmpCpu->regs.A = 0b00010101;
        deExInst(tmpCpu, tmpRam, 0x0F);
        if(!testEval("RRCA", tmpCpu->regs.A == 0b10001010))
            pass = 0;
        
        //test RLA
        tmpCpu->regs.A = 0b10010100;
        resetFlag(tmpCpu, F_C);
        deExInst(tmpCpu, tmpRam, 0x17);
        if(!testEval("RLA", tmpCpu->regs.A == 0b00101000 && getFlag(tmpCpu, F_C)))
            pass = 0;
        
        //test RRA
        tmpCpu->regs.A = 0b00010101;
        resetFlag(tmpCpu, F_C);
        deExInst(tmpCpu, tmpRam, 0x1F);
        if(!testEval("RRA", tmpCpu->regs.A == 0b00001010 && getFlag(tmpCpu, F_C)))
            pass = 0;
        
        //test CPL
        tmpCpu->regs.A = 0b00010101;
        deExInst(tmpCpu, tmpRam, 0x2F);
        if(!testEval("CPL", tmpCpu->regs.A == 0b11101010))
            pass = 0;
        
        //test LD r[y] r[z]
        tmpCpu->regs.D = 0;
        tmpCpu->regs.E = 123;
        deExInst(tmpCpu, tmpRam, 0x53);
        if(!testEval("LD d e", tmpCpu->regs.D == 123))
            pass = 0;
        
        //test ADD
        tmpCpu->regs.A = 255;
        tmpCpu->regs.L = 255;
        resetFlag(tmpCpu, F_C);
        deExInst(tmpCpu, tmpRam, 0x85);
        printf("A : %d , C_F: %d \n",tmpCpu->regs.A, getFlag(tmpCpu, F_C));
        if(!testEval("ADD A L", tmpCpu->regs.A == 254))
            pass = 0;
        
        //test ADC
        tmpCpu->regs.BC = 0;
        tmpCpu->regs.C = tmpCpu->regs.A;
        tmpCpu->regs.A = 0;
        tmpCpu->regs.L = 0;
        deExInst(tmpCpu, tmpRam, 0x8D);
        tmpCpu->regs.B = tmpCpu->regs.A;
        
        printf("BC : %d , C_F: %d \n",tmpCpu->regs.BC, getFlag(tmpCpu, F_C));
        if(!testEval("ADC A L", tmpCpu->regs.BC == 255 + 255))
            pass = 0;
        
        //test SUB
        tmpCpu->regs.A = 0;
        tmpCpu->regs.L = 2;
        resetFlag(tmpCpu, F_C);
        deExInst(tmpCpu, tmpRam, 0x95);
        printf("A : %d , C_F: %d \n",tmpCpu->regs.A, getFlag(tmpCpu, F_C));
        if(!testEval("SUB L", tmpCpu->regs.A == 254))
            pass = 0;
        
        //test SBC
        tmpCpu->regs.BC = 0;
        tmpCpu->regs.C = tmpCpu->regs.A;
        tmpCpu->regs.A = 1;
        tmpCpu->regs.L = 0;
        deExInst(tmpCpu, tmpRam, 0x9D);
        printf("BC : %d , C_F: %d \n",tmpCpu->regs.BC, getFlag(tmpCpu, F_C));
        if(!testEval("SUB L", tmpCpu->regs.BC == 254))
            pass = 0;
        
        
        
            
    }
    //END test
    if(pass)
        printf("--CPU TESTS PASSED--\n");
    else
        printf("--CPU TESTS FAILED--\n");
    
    
    free(tmpRam);
    destroyCPU(&tmpCpu);
}




