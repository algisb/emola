#include "cpu.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "debug.h"
#include "cpu_t.h"
#include "alu.h"
#include "interruptHandling.h"
#include <SDL2/SDL.h>

const uint8_t xMask = 0b11000000;
const uint8_t yMask = 0b00111000;
const uint8_t pMask = 0b00110000;
const uint8_t qMask = 0b00001000;
const uint8_t zMask = 0b00000111;

typedef struct Regs Regs;



#ifdef SHOW_DEBUG_INFO
const char* r_table_dbg[] = {"B", "C", "D", "E", "H", "L", "(HL)", "A"};
const char* rp_table_dbg[] = {"BC", "DE", "HL", "SP"};
const char* rp2_table_dbg[] = {"BC", "DE", "HL", "AF"};
const char* cc_table_dbg[] = {"NZ", "Z", "NC", "C"};
#endif

int logUnhandledOp (int _line, Opcode _opcode)
{
    PRINT_DEBUG("ERROR: unhandled opcode 0x%02x -> %s -> line: %d\n", _opcode.data, __FILE__, _line);
    exit (EXIT_FAILURE);
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

uint8_t * getRVal(CPU * _cpu, uint8_t * _memory, uint8_t _index)//TODO: go through all uses of this and incrament cycles if _index == 6
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
    (*_cpu)->cycles = 0;
    
    (*_cpu)->regs.AF = 0;
    (*_cpu)->regs.BC = 0;
    (*_cpu)->regs.DE = 0;
    (*_cpu)->regs.HL = 0;
    
    (*_cpu)->regs.SP = 0xFFFE;//TODO only here for testing, as stack gets initialized elsewhere
    (*_cpu)->regs.PC = 0;
    (*_cpu)->regs.IR = 0;
    (*_cpu)->regs.IME = 1;
    (*_cpu)->regs.IF = NULL;
    
}

void destroyCPU(CPU ** _cpu)
{
    free(*_cpu);
}

void mapRegisters(CPU* _cpu, uint8_t* _memory)
{
    _cpu->regs.IF = (uint8_t*)(&_memory[0xFF0F]);
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
    
    if(_cpu->regs.PC > (GB_ADDRESS_SPACE_SIZE-1))
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
                            _cpu->cycles += 4;
                            _cpu->regs.PC += 1;
                            break;
                        }
                        case 1:
                        {
                            PRINT_DEBUG("LD (nn), SP\n");
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
                            //_cpu->regs.PC += 2;
                            break;
                        }
                        
                        case 3:
                        {
                            PRINT_DEBUG("JR d\n");
                            int8_t * loc0 = (int8_t *)(&_memory[_cpu->regs.PC + 1]);
                            _cpu->regs.PC += *loc0;
                            _cpu->cycles += 8;
                            break;
                        }

                        case 4:
                        case 5:
                        case 6:
                        case 7:
                        {
                            PRINT_DEBUG("JR %s, d\n", cc_table_dbg[opcode.y - 4]);
                            if(getCC(_cpu, opcode.y - 4))
                            {
                                int8_t * loc0 = (int8_t *)(&_memory[_cpu->regs.PC + 1]);
                                _cpu->regs.PC += *loc0;
                            }
                            else
                            {
                                _cpu->regs.PC += 2;
                            }
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
                
                case 1://z
                {
                    switch(opcode.q)
                    {
                        case 0:
                        {
                            PRINT_DEBUG("LD %s, nn\n", rp_table_dbg[opcode.p]);
                            uint16_t * loc = (uint16_t *)(&_memory[_cpu->regs.PC + 1]);
                            *(_cpu->rptable[opcode.p]) = *loc;
                            
                            _cpu->regs.PC += 3;
                            _cpu->cycles += 12;
                            break;
                        }
                        
                        case 1:
                        {
                            PRINT_DEBUG("ADD HL, %s\n", rp_table_dbg[opcode.p]);
                            addHLuint16(_cpu, _cpu->rptable[opcode.p]);
                            
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
                    opcode.q == 0 ? PRINT_DEBUG("INC %s\n", rp_table_dbg[opcode.p]) : PRINT_DEBUG("DEC %s\n", rp_table_dbg[opcode.p]);
                    uint16_t * v  = _cpu->rptable[opcode.p];
                    opcode.q == 0 ? (*v)++ : (*v)--;
                    _cpu->regs.PC += 1;
                    _cpu->cycles += 8;
                    break;  
                }
                
                case 4://z
                case 5://z
                {
                    opcode.z == 4 ? PRINT_DEBUG("INC %s\n", r_table_dbg[opcode.y]) : PRINT_DEBUG("DEC %s\n", r_table_dbg[opcode.y]);
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
                    PRINT_DEBUG("LD %s, n\n", r_table_dbg[opcode.y]);
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
                //PRINT_DEBUG("HALT\n");
                //exit (EXIT_SUCCESS);
                //TODO
                //halts cpu until and interrupt occurs
                
                //probably loops the func bellow
                //handleInterrupts(_cpu, _memory);
                
                _cpu->cycles += 4;
                //_cpu->regs.PC += 1;
                //while(1);
            }
            else
            {
                PRINT_DEBUG("LD %s, %s\n", r_table_dbg[opcode.y], r_table_dbg[opcode.z]);
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
            //ALU[y] r[z]
            switch(opcode.y)
            {
                case 0:
                {
                    PRINT_DEBUG("ADD %s\n", r_table_dbg[opcode.z]);
                    add(_cpu, getRVal(_cpu, _memory,  opcode.z));
                    
                    _cpu->cycles += opcode.z == 6 ? 8 : 4;
                    _cpu->regs.PC += 1;
                    break;
                }
                case 1:
                {
                    PRINT_DEBUG("ADC %s\n", r_table_dbg[opcode.z]);
                    adc(_cpu, getRVal(_cpu, _memory,  opcode.z));
                    
                    _cpu->cycles += opcode.z == 6 ? 8 : 4;
                    _cpu->regs.PC += 1;
                    break;
                }   
                case 2:
                {
                    PRINT_DEBUG("SUB %s\n", r_table_dbg[opcode.z]);
                    sub(_cpu, getRVal(_cpu, _memory,  opcode.z));
                    
                    _cpu->cycles += opcode.z == 6 ? 8 : 4;
                    _cpu->regs.PC += 1;
                    break;
                }
                case 3:
                {
                    PRINT_DEBUG("SBC %s\n", r_table_dbg[opcode.z]);
                    sbc(_cpu, getRVal(_cpu, _memory,  opcode.z));
                    
                    _cpu->cycles += opcode.z == 6 ? 8 : 4;
                    _cpu->regs.PC += 1;
                    break;
                }
                case 4:
                {
                    PRINT_DEBUG("AND %s\n", r_table_dbg[opcode.z]);
                    and(_cpu, getRVal(_cpu, _memory,  opcode.z));
                    
                    _cpu->cycles += opcode.z == 6 ? 8 : 4;
                    _cpu->regs.PC += 1;
                    break;
                }
                case 5:
                {
                    PRINT_DEBUG("XOR %s\n", r_table_dbg[opcode.z]);
                    xor(_cpu, getRVal(_cpu, _memory,  opcode.z));
                    
                    _cpu->cycles += opcode.z == 6 ? 8 : 4;
                    _cpu->regs.PC += 1;
                    break;
                }
                case 6:
                {
                    PRINT_DEBUG("OR %s\n", r_table_dbg[opcode.z]);
                    or(_cpu, getRVal(_cpu, _memory,  opcode.z));
                    
                    _cpu->cycles += opcode.z == 6 ? 8 : 4;
                    _cpu->regs.PC += 1;
                    break;
                }
                case 7:
                {
                    PRINT_DEBUG("CP %s\n", r_table_dbg[opcode.z]);
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
            switch(opcode.z)
            {
                case 0:
                {
                    switch(opcode.y)
                    {
                        case 0:
                        case 1:
                        case 2:
                        case 3:
                        {
                            PRINT_DEBUG("RET %s\n", cc_table_dbg[opcode.y]);
                            if (getCC(_cpu, opcode.y))
                            {
                                uint16_t * loc = (uint16_t *)(&_memory[_cpu->regs.SP]);
                                _cpu->regs.PC = *loc;
                                _cpu->regs.SP += 2;
                            }
                            
                            _cpu->cycles += 8;
                            break;
                        }
                        
                        case 4:
                        {
                            PRINT_DEBUG("LD (0xFF00 + n), A\n");
                            uint8_t * n = (uint8_t *)(&_memory[_cpu->regs.PC + 1]);
                            uint8_t * loc = (uint8_t *)(&_memory[0xFF00 + *n]);
                            *loc = _cpu->regs.A;
                            _cpu->regs.PC += 2;
                            _cpu->cycles += 12;
                            break;
                        }
                        case 5:
                        {
                            PRINT_DEBUG("ADD SP, d\n");
                            addSPint8(_cpu,(int8_t *)(&_memory[_cpu->regs.PC + 1]));
                            
                            _cpu->regs.PC += 2;
                            _cpu->cycles += 16;
                            break;
                        }
                        case 6:
                        {
                            PRINT_DEBUG("LD A, (0xFF00 + n)\n");
                            uint8_t * n = (uint8_t *)(&_memory[_cpu->regs.PC + 1]);
                            uint8_t * loc = (uint8_t *)(&_memory[0xFF00 + *n]);
                            _cpu->regs.A = *loc;
                            _cpu->regs.PC += 2;
                            _cpu->cycles += 12;
                            break;
                        }
                        case 7:
                        {
                            PRINT_DEBUG("LD HL, SP+d\n");
                            uint16_t tmpSP = _cpu->regs.SP;
                            addSPint8(_cpu,(int8_t *)(&_memory[_cpu->regs.PC + 1]));
                            _cpu->regs.HL = _cpu->regs.SP;
                            _cpu->regs.SP = tmpSP;
                            
                            _cpu->regs.PC += 2;
                            _cpu->cycles += 12;
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
                
                case 1:
                {
                    switch(opcode.q)
                    {
                        case 0:
                        {
                            PRINT_DEBUG("POP %s\n", rp2_table_dbg[opcode.p]);
                            uint16_t * loc = (uint16_t *)(&_memory[_cpu->regs.SP]);
                            *_cpu->rp2table[opcode.p] = *loc;
                            _cpu->regs.SP += 2;
                            _cpu->regs.PC += 1;
                            _cpu->cycles += 12;
                            break;
                        }
                        case 1:
                        {
                            switch(opcode.p)
                            {
                                case 0:
                                {
                                    PRINT_DEBUG("RET\n");
                                    uint16_t * loc = (uint16_t *)(&_memory[_cpu->regs.SP]);
                                    _cpu->regs.PC = *loc;
                                    _cpu->regs.SP += 2;
                                    
                                    _cpu->regs.PC += 1;
                                    _cpu->cycles += 16;
                                    break;
                                }
                                case 1:
                                {
                                    PRINT_DEBUG("RETI\n");
                                    uint16_t * loc = (uint16_t *)(&_memory[_cpu->regs.SP]);
                                    _cpu->regs.PC = *loc;
                                    _cpu->regs.SP += 2;
                                    
                                    _cpu->regs.IME = 1;
                                    
                                    _cpu->cycles += 16;
                                    break;
                                }
                                case 2:
                                {
                                    PRINT_DEBUG("JP HL\n");
                                    _cpu->regs.PC = _cpu->regs.HL;
                                    _cpu->cycles += 4;
                                    break;
                                }
                                case 3:
                                {
                                    PRINT_DEBUG("LD SP, HL\n");
                                    _cpu->regs.SP = _cpu->regs.HL;
                                                                        
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
                        default:
                        {
                            LOG_ERROR_OP(opcode);
                            break;
                        }
                        
                    }
                    break;
                }
                
                case 2://z
                {
                    switch(opcode.y)
                    {
                        case 0:
                        case 1:
                        case 2:
                        case 3:
                        {
                            PRINT_DEBUG("JP %s, nn\n", cc_table_dbg[opcode.y]);
                            if (getCC(_cpu, opcode.y))
                            {
                                uint16_t * loc = (uint16_t *)(&_memory[_cpu->regs.PC + 1]);
                                _cpu->regs.PC = *loc;
                            }
                            else
                            {
                                _cpu->regs.PC += 3;
                            }
                            _cpu->cycles += 12;
                            break;
                        }
                        
                        case 4:
                        {
                            PRINT_DEBUG("LD (0xFF00+C), A\n");
                            uint8_t * loc = (uint8_t *)(&_memory[0xFF00 + _cpu->regs.C]);
                            *loc = _cpu->regs.A;
                            
                            _cpu->regs.PC += 2;
                            _cpu->cycles += 8;
                            break;
                        }
                        
                        case 5:
                        {
                            PRINT_DEBUG("LD (nn), A\n");
                            uint16_t * nn = (uint16_t *)(&_memory[_cpu->regs.PC + 1]);
                            uint8_t * loc = (uint8_t *)(&_memory[*nn]);
                            *loc = _cpu->regs.A;
                            
                            _cpu->regs.PC += 3;
                            _cpu->cycles += 16;
                            break;
                        }
                        
                        case 6:
                        {
                            PRINT_DEBUG("LD A, (0xFF00+C)\n");
                            uint8_t * loc = (uint8_t *)(&_memory[0xFF00 + _cpu->regs.C]);
                            _cpu->regs.A = *loc;
                            
                            _cpu->regs.PC += 2;
                            _cpu->cycles += 8;
                            break;
                        }
                        
                        case 7:
                        {
                            PRINT_DEBUG("LD A, (nn)\n");
                            uint16_t * nn = (uint16_t *)(&_memory[_cpu->regs.PC + 1]);
                            uint8_t * loc = (uint8_t *)(&_memory[*nn]);
                            _cpu->regs.A = *loc;
                            
                            _cpu->regs.PC += 3;
                            _cpu->cycles += 16;
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
                    switch(opcode.y)
                    {
                        case 0:
                        {
                            PRINT_DEBUG("JP nn\n");
                            uint16_t * nn = (uint16_t *)(&_memory[_cpu->regs.PC + 1]);
                            _cpu->regs.PC = *nn;
                            
                            //_cpu->regs.PC += 3;
                            _cpu->cycles += 16;
                            break;
                        }
                        
                        case 6:
                        {
                            PRINT_DEBUG("DI\n");
                            
                            _cpu->regs.IME = 0;
                            _cpu->regs.PC += 1;
                            _cpu->cycles += 4;
                            break;
                        }
                        
                        case 7:
                        {
                            PRINT_DEBUG("EI\n");
                            
                            _cpu->regs.IME = 1;
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
                
                case 4://z
                {
                    switch(opcode.y)
                    {
                        case 0:
                        case 1:
                        case 2:
                        case 3:
                        {
                            PRINT_DEBUG("CALL %s, nn\n", cc_table_dbg[opcode.y]);
                            if (getCC(_cpu, opcode.y))
                            {
                                //push
                                _cpu->regs.SP -= 2;
                                uint16_t * loc = (uint16_t *)(&_memory[_cpu->regs.SP]);
                                *loc = (_cpu->regs.PC+3);
                                //jump to label
                                uint16_t * nn = (uint16_t *)(&_memory[_cpu->regs.PC + 1]);
                                _cpu->regs.PC = *nn;
                            
                                _cpu->regs.PC += 3;
                                _cpu->cycles += 24;
                            }
                            else
                            {
                                _cpu->regs.PC += 3;
                                _cpu->cycles += 12;
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
                
                case 5://z
                {
                    switch(opcode.q)//q
                    {
                        case 0:
                        {
                            PRINT_DEBUG("PUSH %s\n", rp2_table_dbg[opcode.p]);
                            _cpu->regs.SP -= 2;
                            uint16_t * loc = (uint16_t *)(&_memory[_cpu->regs.SP]);
                            *loc = *_cpu->rp2table[opcode.p];
                            _cpu->regs.PC += 1;
                            _cpu->cycles += 16;
                            break;
                        }
                        
                        case 1:
                        {
                            switch(opcode.p)
                            {
                                case 0:
                                {
                                    PRINT_DEBUG("CALL nn\n");
                                    //push
                                    _cpu->regs.SP -= 2;
                                    uint16_t * loc = (uint16_t *)(&_memory[_cpu->regs.SP]);
                                    *loc = (_cpu->regs.PC+3);
                                    //jump to label
                                    uint16_t * nn = (uint16_t *)(&_memory[_cpu->regs.PC + 1]);
                                    _cpu->regs.PC = *nn;
                                    
                                    _cpu->regs.PC += 3;
                                    _cpu->cycles += 24;
                                    
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
                
                case 6:
                {
                    //ALU[y] n
                    switch(opcode.y)
                    {
                        case 0:
                        {
                            PRINT_DEBUG("ADD n\n");
                            uint8_t * n = (uint8_t *)(&_memory[_cpu->regs.PC + 1]);
                            add(_cpu, n);
                            
                            _cpu->cycles += 8;
                            _cpu->regs.PC += 2;
                            break;
                        }
                        case 1:
                        {
                            PRINT_DEBUG("ADC n\n");
                            uint8_t * n = (uint8_t *)(&_memory[_cpu->regs.PC + 1]);
                            adc(_cpu, n);
                            
                            _cpu->cycles += 8;
                            _cpu->regs.PC += 2;
                            break;
                        }   
                        case 2:
                        {
                            PRINT_DEBUG("SUB n\n");
                            uint8_t * n = (uint8_t *)(&_memory[_cpu->regs.PC + 1]);
                            sub(_cpu, n);
                            
                            _cpu->cycles += 8;
                            _cpu->regs.PC += 2;
                            break;
                        }
                        case 3:
                        {
                            PRINT_DEBUG("SBC n\n");
                            uint8_t * n = (uint8_t *)(&_memory[_cpu->regs.PC + 1]);
                            sbc(_cpu, n);
                            
                            _cpu->cycles += 8;
                            _cpu->regs.PC += 2;
                            break;
                        }
                        case 4:
                        {
                            PRINT_DEBUG("AND n\n");
                            uint8_t * n = (uint8_t *)(&_memory[_cpu->regs.PC + 1]);
                            and(_cpu, n);
                            
                            _cpu->cycles += 8;
                            _cpu->regs.PC += 2;
                            break;
                        }
                        case 5:
                        {
                            PRINT_DEBUG("XOR n\n");
                            uint8_t * n = (uint8_t *)(&_memory[_cpu->regs.PC + 1]);
                            xor(_cpu, n);
                            
                            _cpu->cycles += 8;
                            _cpu->regs.PC += 2;
                            break;
                        }
                        case 6:
                        {
                            PRINT_DEBUG("OR n\n");
                            uint8_t * n = (uint8_t *)(&_memory[_cpu->regs.PC + 1]);
                            or(_cpu, n);
                            
                            _cpu->cycles += 8;
                            _cpu->regs.PC += 2;
                            break;
                        }
                        case 7:
                        {
                            PRINT_DEBUG("AND n\n");
                            uint8_t * n = (uint8_t *)(&_memory[_cpu->regs.PC + 1]);
                            cp(_cpu, n);
                            
                            _cpu->cycles += 8;
                            _cpu->regs.PC += 2;
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
                
                case 7:
                {
                    PRINT_DEBUG("RST y*8\n");
                    //push
                    _cpu->regs.SP -= 2;
                    uint16_t * loc = (uint16_t *)(&_memory[_cpu->regs.SP]);
                    *loc = _cpu->regs.PC + 1;
                    //jump
                    _cpu->regs.PC = opcode.y * 8;
                    
                    _cpu->cycles += 16;
                    
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

}

void deExInstPrefixed(CPU * _cpu, uint8_t * _memory, uint8_t _op)
{
    Opcode opcode = decodeOp(_op);
    switch(opcode.x)
    {
        case 0:
        {
            switch(opcode.y)
            {
                case 0:
                {
                    PRINT_DEBUG("RLC r[z]\n");
                    uint8_t* r = getRVal(_cpu, _memory, opcode.z);
                    uint8_t msb = *r & 0b10000000;
                    
                    *r = (*r << 1);
                    if (msb)
                    {
                        *r |= 0b00000001;
                        setFlag(_cpu, F_C);
                    }
                    else
                    {
                        *r &= ~(0b00000001);
                        resetFlag(_cpu, F_C);
                    }
                    
                    *r ? resetFlag(_cpu, F_Z) : setFlag(_cpu, F_Z);
                    resetFlag(_cpu, F_H);
                    resetFlag(_cpu, F_N);
                    
                    _cpu->regs.PC += 1;
                    _cpu->cycles += opcode.z == 6 ? 16 : 8;
                    break;
                }
                case 1:
                {
                    PRINT_DEBUG("RRC r[z]\n");
                    
                    uint8_t* r = getRVal(_cpu, _memory, opcode.z);
                    uint8_t lsb = *r & 0b00000001;
                    
                    *r = (*r >> 1);
                    if (lsb)
                    {
                        *r |= 0b10000000;
                        setFlag(_cpu, F_C);
                    }
                    else
                    {
                        *r &= ~(0b10000000);
                        resetFlag(_cpu, F_C);
                    }
                    
                    *r ? resetFlag(_cpu, F_Z) : setFlag(_cpu, F_Z);
                    resetFlag(_cpu, F_H);
                    resetFlag(_cpu, F_N);
                    
                    _cpu->regs.PC += 1;
                    _cpu->cycles += opcode.z == 6 ? 16 : 8;
                    break;
                }
                case 2:
                {
                    PRINT_DEBUG("RL r[z]\n");
                    
                    uint8_t* r = getRVal(_cpu, _memory, opcode.z);
                    uint8_t msb = *r & 0b10000000;
                    uint8_t cf = getFlag(_cpu, F_C);
                    
                    *r = (*r << 1);
                    msb ? setFlag(_cpu, F_C) : resetFlag(_cpu, F_C);
                    cf ? (*r |= 0b00000001) : (*r &= ~(0b00000001));
                    *r ? resetFlag(_cpu, F_Z) : setFlag(_cpu, F_Z);
                    
                    resetFlag(_cpu, F_H);
                    resetFlag(_cpu, F_N);
                    
                    _cpu->regs.PC += 1;
                    _cpu->cycles += opcode.z == 6 ? 16 : 8;
                    break;
                }
                case 3:
                {
                    PRINT_DEBUG("RR r[z]\n");
                    
                    uint8_t* r = getRVal(_cpu, _memory, opcode.z);
                    uint8_t lsb = *r & 0b00000001;
                    uint8_t cf = getFlag(_cpu, F_C);
                    
                    *r = (*r >> 1);
                    lsb ? setFlag(_cpu, F_C) : resetFlag(_cpu, F_C);
                    cf ? (*r |= 0b10000000) : (*r &= ~(0b10000000));
                    *r ? resetFlag(_cpu, F_Z) : setFlag(_cpu, F_Z);
                    
                    resetFlag(_cpu, F_H);
                    resetFlag(_cpu, F_N);
                    
                    _cpu->regs.PC += 1;
                    _cpu->cycles += opcode.z == 6 ? 16 : 8;
                    break;
                    
                }
                
                case 4:
                {
                    PRINT_DEBUG("SLA r[z]\n");
                    
                    uint8_t* r = getRVal(_cpu, _memory, opcode.z);
                    uint8_t msb = *r & 0b10000000;
                    
                    *r = (*r << 1);
                    msb ? setFlag(_cpu, F_C) : resetFlag(_cpu, F_C);
                    *r ? resetFlag(_cpu, F_Z) : setFlag(_cpu, F_Z);
                    
                    resetFlag(_cpu, F_H);
                    resetFlag(_cpu, F_N);
                    
                    _cpu->regs.PC += 1;
                    _cpu->cycles += opcode.z == 6 ? 16 : 8;
                    break;
                }
                case 5:
                {
                    PRINT_DEBUG("SRA r[z]\n");
                    
                    uint8_t* r = getRVal(_cpu, _memory, opcode.z);
                    uint8_t lsb = *r & 0b00000001;
                    uint8_t msb = *r & 0b10000000;
                    
                    *r = (*r >> 1);
                    *r |= msb;
                    
                    lsb ? setFlag(_cpu, F_C) : resetFlag(_cpu, F_C);
                    *r ? resetFlag(_cpu, F_Z) : setFlag(_cpu, F_Z);
                    
                    resetFlag(_cpu, F_H);
                    resetFlag(_cpu, F_N);
                    
                    _cpu->regs.PC += 1;
                    _cpu->cycles += opcode.z == 6 ? 16 : 8;
                    break;
                }
                case 6:
                {
                    PRINT_DEBUG("SWAP r[z]\n");
                    
                    uint8_t* r = getRVal(_cpu, _memory, opcode.z);
                    uint8_t un = *r & 0xf0;
                    un = un >> 4;
                    *r = *r << 4;
                    *r = *r | un;
                    *r ? resetFlag(_cpu, F_Z) : setFlag(_cpu, F_Z);
                    
                    resetFlag(_cpu, F_H);
                    resetFlag(_cpu, F_N);
                    resetFlag(_cpu, F_C);
                    
                    _cpu->regs.PC += 1;
                    _cpu->cycles += opcode.z == 6 ? 16 : 8;
                    break;
                }
                case 7:
                {
                    PRINT_DEBUG("SRL r[z]\n");
                    
                    uint8_t* r = getRVal(_cpu, _memory, opcode.z);
                    uint8_t lsb = *r & 0b00000001;
                    
                    *r = (*r >> 1);
                    
                    lsb ? setFlag(_cpu, F_C) : resetFlag(_cpu, F_C);
                    *r ? resetFlag(_cpu, F_Z) : setFlag(_cpu, F_Z);
                    
                    resetFlag(_cpu, F_H);
                    resetFlag(_cpu, F_N);
                    
                    _cpu->regs.PC += 1;
                    _cpu->cycles += opcode.z == 6 ? 16 : 8;
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
        
        case 1:
        {
            PRINT_DEBUG("BIT y, r[z]\n");
            uint8_t* r = getRVal(_cpu, _memory, opcode.z);
            uint8_t mask = 1 << opcode.y;
            
            *r & mask ? resetFlag(_cpu, F_Z) : setFlag(_cpu, F_Z);
             
            resetFlag(_cpu, F_N);
            setFlag(_cpu, F_H);
            _cpu->regs.PC += 1;
            _cpu->cycles += 8;
            break;
        }
        case 2:
        {
            PRINT_DEBUG("RES y, r[z]\n");
            uint8_t* r = getRVal(_cpu, _memory, opcode.z);
            uint8_t mask = 1 << opcode.y;
            *r &= ~(mask);
            
            _cpu->regs.PC += 1;
            _cpu->cycles += 8;
            break;
        }
        case 3:
        {
            PRINT_DEBUG("SET y, r[z]\n");
            uint8_t* r = getRVal(_cpu, _memory, opcode.z);
            uint8_t mask = 1 << opcode.y;
            *r |= mask;
            
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
}

void feDeExInst(CPU * _cpu, uint8_t * _memory)
{
    fetchByte(_cpu, _memory);
    
    
    //TODO halt and stop will need special handling
    //TODO DI and EI only should come in effect after the next instruction
    if(_cpu->regs.IME == 1)
    {
        //interrupt handling
        handleInterrupts(_cpu, _memory);
    }
    if(_cpu->regs.IR == 0xCB)//prefix byte, means using different instructions
    {
        _cpu->regs.PC += 1;
        fetchByte(_cpu, _memory);//fetch another instruction as prefix byte was present
        deExInstPrefixed(_cpu, _memory, _cpu->regs.IR);
    }
    else //normal execution
    {
        deExInst(_cpu, _memory, _cpu->regs.IR);
    }
    
}

int testEval(char * _testName, int _testCondition)
{
    if(_testCondition)
    {
        PRINT_DEBUG("test %s - PASS\n", _testName);
        return 1;
    }
    else
    {
        PRINT_DEBUG("test %s - FAIL\n", _testName);
        return 0;
    }
}

void runTestsCPU()
{
    CPU * tmpCpu;
    createCPU(&tmpCpu);
    uint8_t * tmpRam = (uint8_t*)calloc(GB_ADDRESS_SPACE_SIZE, 1);
    
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
        
        //test PUSH BC
        tmpCpu->regs.BC = 100;
        deExInst(tmpCpu, tmpRam, 0xC5);
        uint16_t * loc = (uint16_t *)(&tmpRam[tmpCpu->regs.SP]);
        printf("TOP STACK VALUE : %d \n", *loc);
        if(!testEval("PUSH BC", *loc == 100))
            pass = 0;
        
        //test POP DE
        deExInst(tmpCpu, tmpRam, 0xD1);
        printf("DE : %d \n", tmpCpu->regs.DE);
        if(!testEval("POP DE", tmpCpu->regs.DE == 100))
            pass = 0;
        
        //test ADD SP, d
        {
        uint16_t tmpSP = tmpCpu->regs.SP;
        tmpCpu->regs.SP = 0;
        int8_t * d = (int8_t*)(&tmpRam[tmpCpu->regs.PC + 1]);
        *d = -100;
        deExInst(tmpCpu, tmpRam, 0xE8);
        printf("SP: %d C_F: %d \n", tmpCpu->regs.SP, getFlag(tmpCpu,F_C));
        if(!testEval("ADD SP, d", (tmpCpu->regs.SP == 0xFFFF - 99) && (getFlag(tmpCpu,F_C)) ))
            pass = 0;
        tmpCpu->regs.SP = tmpSP;
        }
        
        //test CALL
        {
        uint16_t tmpPC = 590;
        tmpCpu->regs.PC = tmpPC;
        uint16_t * loc3 = (uint16_t *)(&tmpRam[tmpCpu->regs.PC + 1]);
        *loc3 = 690;
        
        uint8_t * loc2 = (uint8_t *)(&tmpRam[690]);
        *loc2 = 0x0;
        
        deExInst(tmpCpu, tmpRam, 0xCD);
        deExInst(tmpCpu, tmpRam, 0xC9);
        printf("PC: %d \n", tmpCpu->regs.PC);
        //tmpRam[tmpCpu->regs.SP];
        if(!testEval("CALL 690", tmpCpu->regs.PC == tmpPC + 4))
            pass = 0;
        }
        
        //test RST
        {
        deExInst(tmpCpu, tmpRam, 0xCF);
        if(!testEval("RST 08H", tmpCpu->regs.PC == 0x08))
            pass = 0;
        
        
        deExInst(tmpCpu, tmpRam, 0xEF);
        if(!testEval("RST 28H", tmpCpu->regs.PC == 0x28))
            pass = 0;
        
        deExInst(tmpCpu, tmpRam, 0xE7);
        if(!testEval("RST 20H", tmpCpu->regs.PC == 0x20))
            pass = 0;
        }
        
        //test ADD d8
        {
            tmpCpu->regs.PC = 0;
            tmpCpu->regs.A = 0;
            
            int8_t * d = (int8_t*)(&tmpRam[tmpCpu->regs.PC + 1]);
            *d = 30;
            
            deExInst(tmpCpu, tmpRam, 0xC6);
            if(!testEval("ADD A, 30", tmpCpu->regs.A == 30))
                pass = 0;
            
        }
        
        //test RLC
        {
            tmpCpu->regs.B = 0b10000000;
            deExInstPrefixed(tmpCpu, tmpRam, 0x00);
            if(!testEval("RLC B", tmpCpu->regs.B == 1))
                pass = 0;
        }
        
        //test RL
        {
            setFlag(tmpCpu, F_C);
            tmpCpu->regs.B = 0b10000001;
            deExInstPrefixed(tmpCpu, tmpRam, 0x10);
            if(!testEval("RL B", tmpCpu->regs.B == 3 && getFlag(tmpCpu, F_C) == 1))
                pass = 0;
        }
        //test RR
        {
            setFlag(tmpCpu, F_C);
            tmpCpu->regs.B = 0b10000001;
            deExInstPrefixed(tmpCpu, tmpRam, 0x18);
            if(!testEval("RR B", tmpCpu->regs.B == 0b11000000 && getFlag(tmpCpu, F_C) == 1))
                pass = 0;
        }
        
        //test SLA
        {
            resetFlag(tmpCpu, F_C);
            tmpCpu->regs.B = 0b10000001;
            deExInstPrefixed(tmpCpu, tmpRam, 0x20);
            if(!testEval("SLA B", tmpCpu->regs.B == 0b00000010 && getFlag(tmpCpu, F_C) == 1))
                pass = 0;
        }
        
        //test SRA
        {
            resetFlag(tmpCpu, F_C);
            tmpCpu->regs.B = 0b10000001;
            deExInstPrefixed(tmpCpu, tmpRam, 0x28);
            if(!testEval("SRA B", tmpCpu->regs.B == 0b11000000 && getFlag(tmpCpu, F_C) == 1))
                pass = 0;
        }
        
        //test SWAP
        {
            tmpCpu->regs.B = 0xf0;
            deExInstPrefixed(tmpCpu, tmpRam, 0x30);
            if(!testEval("SWAP B", tmpCpu->regs.B == 0x0f))
                pass = 0;
            
            tmpCpu->regs.B = 0x0f;
            deExInstPrefixed(tmpCpu, tmpRam, 0x30);
            if(!testEval("SWAP B", tmpCpu->regs.B == 0xf0))
                pass = 0;
        }
        
        //test SRL
        {
            resetFlag(tmpCpu, F_C);
            tmpCpu->regs.B = 0b10000001;
            deExInstPrefixed(tmpCpu, tmpRam, 0x38);
            if(!testEval("SRL B", tmpCpu->regs.B == 0b01000000 && getFlag(tmpCpu, F_C) == 1))
                pass = 0;
        }
        //test BIT
        {
            resetFlag(tmpCpu, F_Z);
            tmpCpu->regs.B = 0b11111110;
            deExInstPrefixed(tmpCpu, tmpRam, 0x40);
            if(!testEval("BIT 0, B", getFlag(tmpCpu, F_Z) == 1))
                pass = 0;
            
            setFlag(tmpCpu, F_Z);
            tmpCpu->regs.B = 0b00000001;
            deExInstPrefixed(tmpCpu, tmpRam, 0x40);
            if(!testEval("BIT 0, B", getFlag(tmpCpu, F_Z) == 0))
                pass = 0;
            
            
            setFlag(tmpCpu, F_Z);
            tmpCpu->regs.B = 0b00001000;
            deExInstPrefixed(tmpCpu, tmpRam, 0x58);
            if(!testEval("BIT 4, B", getFlag(tmpCpu, F_Z) == 0))
                pass = 0;
        }
        
            
    }
    //END test
    if(pass)
        PRINT_DEBUG("--CPU TESTS PASSED--\n");
    else
        PRINT_DEBUG("--CPU TESTS FAILED--\n");
    
    //SDL_Delay(3000);
    free(tmpRam);
    destroyCPU(&tmpCpu);
}




