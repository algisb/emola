#include "mmu.h"
#include <stdlib.h>
#include <stdio.h>
#include "defs.h"

int createRAM(unsigned char ** _ram)
{
    *_ram = (unsigned char*)calloc(RAM_SIZE, 1);
    unsigned char * ram = *_ram;
    int i = 0;
    //instruction 1 (NOP)
    ram[i] = 0x00;
    i++;
    //instruction 2 (LD (**) SP) 
    ram[i] = 0x08;
    i++;
    unsigned short * tmp = (unsigned short*)(ram + i);
    *tmp = 69;
    i+=2;
    
    
    
    //instruction 3(HALT)
    ram[i] = 0x76;
    i++;
    
    printf("program in memory:\n");
    for(int j = 0; j<i; j++)
        printf("0x%02x ",ram[j]);
    printf("\n");
    printf("-------------------------------\n");
    
    
    
    
    return 0;
}

