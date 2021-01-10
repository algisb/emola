#include "alu.h"
#include "cpu_t.h"

void add(CPU * _cpu, uint8_t * _n )
{
    
    //process half carry flag
    ((_cpu->regs.A & 0x0F) + (*_n & 0x0F) ) & 0xF0 ? 
    setFlag(_cpu, F_H) : resetFlag(_cpu, F_H);

    //process carry flag
    uint16_t c = ((_cpu->regs.A) + (*_n));
    c > 255 ? setFlag(_cpu, F_C) : resetFlag(_cpu, F_C);
    
     
     _cpu->regs.A += *_n;
     _cpu->regs.A ? resetFlag(_cpu, F_Z) : setFlag(_cpu, F_Z);
     resetFlag(_cpu, F_N);
}

void adc(CPU * _cpu, uint8_t * _n )
{
    uint8_t carryFlag = getFlag(_cpu, F_C);
    //process half carry flag
    ((_cpu->regs.A & 0x0F) + ((*_n + carryFlag)  & 0x0F) ) & 0xF0 ? 
    setFlag(_cpu, F_H) : resetFlag(_cpu, F_H);

    //process carry flag
    uint16_t c = ((_cpu->regs.A) + ((*_n + carryFlag)));
    c > 255 ? setFlag(_cpu, F_C) : resetFlag(_cpu, F_C);
    
     
    _cpu->regs.A += *_n + carryFlag;
    _cpu->regs.A ? resetFlag(_cpu, F_Z) : setFlag(_cpu, F_Z);
    resetFlag(_cpu, F_N);
}

void sub(CPU * _cpu, uint8_t * _n )
{
    //process half carry flag
    (_cpu->regs.A & 0x0F) < (*_n & 0x0F)  ? 
    setFlag(_cpu, F_H) : resetFlag(_cpu, F_H);

    //process carry flag
    (_cpu->regs.A) < (*_n) ?
    setFlag(_cpu, F_C) : resetFlag(_cpu, F_C);
     
     _cpu->regs.A -= *_n;
     _cpu->regs.A ? resetFlag(_cpu, F_Z) : setFlag(_cpu, F_Z);
     setFlag(_cpu, F_N);
}

void sbc(CPU * _cpu, uint8_t * _n )
{
    uint8_t carryFlag = getFlag(_cpu, F_C);
        
    //process half carry flag
    (_cpu->regs.A & 0x0F) < ((*_n + carryFlag) & 0x0F)  ? 
    setFlag(_cpu, F_H) : resetFlag(_cpu, F_H);

    //process carry flag
    (_cpu->regs.A) < (*_n + carryFlag) ?
    setFlag(_cpu, F_C) : resetFlag(_cpu, F_C);
     
     _cpu->regs.A -= (*_n + carryFlag);
     _cpu->regs.A ? resetFlag(_cpu, F_Z) : setFlag(_cpu, F_Z);
     setFlag(_cpu, F_N);
}

void and(CPU * _cpu,  uint8_t * _n )
{
    _cpu->regs.A  = _cpu->regs.A & *_n;
    
    _cpu->regs.A ? resetFlag(_cpu, F_Z) : setFlag(_cpu, F_Z);
    resetFlag(_cpu, F_N);
    setFlag(_cpu, F_H);
    resetFlag(_cpu, F_C);
}

void or(CPU * _cpu,  uint8_t * _n )
{
    _cpu->regs.A  = _cpu->regs.A | *_n;
    
    _cpu->regs.A ? resetFlag(_cpu, F_Z) : setFlag(_cpu, F_Z);
    resetFlag(_cpu, F_N);
    resetFlag(_cpu, F_H);
    resetFlag(_cpu, F_C);
}

void xor(CPU * _cpu,  uint8_t * _n )
{
    _cpu->regs.A  = _cpu->regs.A ^ *_n;
    
    _cpu->regs.A ? resetFlag(_cpu, F_Z) : setFlag(_cpu, F_Z);
    resetFlag(_cpu, F_N);
    resetFlag(_cpu, F_H);
    resetFlag(_cpu, F_C);
}

void cp(CPU * _cpu, uint8_t * _n )
{
    //process half carry flag
    (_cpu->regs.A & 0x0F) < (*_n & 0x0F)  ? 
    setFlag(_cpu, F_H) : resetFlag(_cpu, F_H);

    //process carry flag
    (_cpu->regs.A) < (*_n) ?
    setFlag(_cpu, F_C) : resetFlag(_cpu, F_C);
     
     //_cpu->regs.A -= *_n;
     _cpu->regs.A ==  *_n ? setFlag(_cpu, F_Z) : resetFlag(_cpu, F_Z);
     setFlag(_cpu, F_N);
}




void addHLuint16(CPU * _cpu,  uint16_t * _n )
{
    resetFlag(_cpu, F_N);
                            
    //process half carry flag
    ((_cpu->regs.HL & 0x0FFF) + (*(_n) & 0x0FFF )) & 0xF000 ? 
    setFlag(_cpu, F_H) : resetFlag(_cpu, F_H);
    
    //process carry flag
    uint32_t c = (_cpu->regs.HL) + *(_n); 
    c > 0xFFFF ? setFlag(_cpu, F_C) : resetFlag(_cpu, F_C);
    
    _cpu->regs.HL += *(_n);
                            
}

void addSPint8(CPU * _cpu,  int8_t * _n )
{
    int8_t * d = _n;
    uint8_t sign = (*d) & 0b10000000;
    uint8_t ud = sign ? 128 - (*d) & 0b01111111 : (*d) & 0b01111111;
    if(sign)//neg
    {
        //process half carry flag
        ((_cpu->regs.SP & 0x0FFF) < ud) ? 
        setFlag(_cpu, F_H) : resetFlag(_cpu, F_H);
        
        //process carry flag
        (_cpu->regs.SP) < ud ?
        setFlag(_cpu, F_C) : resetFlag(_cpu, F_C);
        
    }
    else//pos
    {
        //process half carry flag
        ((_cpu->regs.SP & 0x0FFF) + *(d)) & 0xF000 ? 
        setFlag(_cpu, F_H) : resetFlag(_cpu, F_H);
        
        //process carry flag
        uint32_t c = (_cpu->regs.SP) + *(d); 
        c > 0xFFFF ? setFlag(_cpu, F_C) : resetFlag(_cpu, F_C);
        
    }
    _cpu->regs.SP += *d;
    resetFlag(_cpu, F_Z);
    resetFlag(_cpu, F_N);
}

