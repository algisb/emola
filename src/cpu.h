#ifndef CPU_H_
#define CPU_H_
#include "regs.h"
typedef struct
{
    Regs regs;
    unsigned char PC;
    unsigned char Flags;
}CPU;

#endif
