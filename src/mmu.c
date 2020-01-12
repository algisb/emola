#include "mmu.h"
#include <stdlib.h>
#include <stdio.h>
#include "defs.h"

void createRAM(uint8_t ** _ram)
{
    *_ram = (uint8_t*)calloc(RAM_SIZE, 1);
    uint8_t * ram = *_ram;
    uint16_t * tmp_uint16; 
    int i = 0;
    //instruction 1 (NOP)
    ram[i] = 0x00;
    i++;
    //instruction 2 (LD (**) SP) 
    ram[i] = 0x08;
    i++;
    tmp_uint16 = (uint16_t*)(ram + i);
    *tmp_uint16 = 69;
    i+=2;
    //instruction 3 (JR d)
    ram[i] = 0x28;
    i++;
    ram[i] = -6;
    i++;
    
    //instruction 4 LD HL dd
    ram[i] = 0x21;
    i++;
    tmp_uint16 = (uint16_t*)(ram + i);
    *tmp_uint16 = 799;
    i+=2;
    
    //instruction LD (HL+), A
    ram[i] = 0x22;
    i++;
    
    
    //HALT
    ram[i] = 0x76;
    i++;
    
    printf("program in memory:\n");
    for(int j = 0; j<i; j++)
        printf("0x%02x ",ram[j]);
    printf("\n");
    printf("-------------------------------\n");
    

}

