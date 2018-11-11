#include "regs.h"
#include <stdlib.h>
#include <stdio.h>

int resetRegs(Regs * _regs)
{
    _regs->IR = 0;
    _regs->PC = 0;
    return 0;
}

