#include "mmu.h"
#include <stdlib.h>
#include <stdio.h>
#include "defs.h"

int createRAM(unsigned char ** _ram)
{
    *_ram = (unsigned char*)calloc(RAM_SIZE, 1);
    unsigned char * ram = *_ram;
    //instruction 1 (NOP)
    ram[0] = 0x00;
    //instruction 1(LD B 69)
    ram[1] = 0x06;
    ram[2] = 69;
    //instruction 2(LD H 69)
    ram[3] = 0x26;
    ram[4] = 69;
    //instruction 3(HALT)
    ram[5] = 0x76;
    
    
    return 0;
}

