#ifndef CPU_T_H_
#define CPU_T_H_

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

#endif
