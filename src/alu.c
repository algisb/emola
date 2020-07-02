#include "alu.h"
#include "cpu_t.h"

void add(CPU * _cpu, uint8_t * _n )
{
    _cpu->regs.A += *_n;
    
    //process half carry flag
    ((_cpu->regs.A & 0x0F) + (*_n & 0x000F) ) & 0xF0 ? 
    setFlag(_cpu, F_H) : resetFlag(_cpu, F_H);

    //process carry flag
    uint16_t c = ((_cpu->regs.A & 0xFF) + (*_n  & 0xFF )) & 0xF00;
     c ? setFlag(_cpu, F_C) : resetFlag(_cpu, F_C);
}
