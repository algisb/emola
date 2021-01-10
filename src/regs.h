#ifndef REGISTER_H_
#define REGISTER_H_
#include <stdint.h>
struct Regs
{
    union
    {   
        uint16_t AF;
        struct
        {
            uint8_t F; //Flag register
            uint8_t A; //Hi
        };
    };

    union
    {
        uint16_t BC;
        struct
        {
            uint8_t C;
            uint8_t B;
        };
        
    };
    union
    {
        uint16_t DE;
        struct
        {
            uint8_t E;
            uint8_t D;
        };
    };
    union
    {
        uint16_t HL;
        struct
        {
            uint8_t L;
            uint8_t H;
        };
    };

    uint16_t SP; //Stack pointer

    uint16_t PC; //Program counter

    uint8_t IR; //Instruction register
    
    uint8_t IME; //Interrupt master enable flag
    
    uint8_t* IF; //memory mapped interrup request register

};
typedef struct Regs Regs;

int resetRegs(Regs * _regs);
#endif
