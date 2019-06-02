#include "mmu.h"
#include <stdlib.h>
#include <stdio.h>
#include "defs.h"

int createRAM(unsigned char ** _ram)
{
    *_ram = (unsigned char*)calloc(RAM_SIZE, 1);
    unsigned char * ram = *_ram;
    ram[1] = 0x01;
}

