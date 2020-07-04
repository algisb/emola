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

