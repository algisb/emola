#include "mmu.h"
#include <stdlib.h>
#include <stdio.h>
#include "defs.h"

void createRAM(uint8_t ** _ram)
{
    *_ram = (uint8_t*)calloc(GB_ADDRESS_SPACE_SIZE, 1);
    uint8_t * ram = *_ram;
    uint16_t * tmp_uint16; 
    int i = 0;
    //NOP
    ram[i] = 0x00;
    i++;
    //LD (**) SP 
    ram[i] = 0x08;
    i++;
    tmp_uint16 = (uint16_t*)(ram + i);
    *tmp_uint16 = 69;
    i+=2;
    
    //LD HL dd
    ram[i] = 0x21;
    i++;
    tmp_uint16 = (uint16_t*)(ram + i);
    *tmp_uint16 = 799;
    i+=2;
    
    //LD (HL+), A
    ram[i] = 0x22;
    i++;
    
    //LD A, (BC)
    ram[i] = 0x0A;
    i++;
    
    //inc D
    ram[i] = 0x14;
    i++;

    //inc BC
    ram[i] = 0x03;
    i++;    
    
    //LD B, dd
    ram[i] = 0x06;
    i++;
    ram[i] = 111;
    i++;
    
    //RLC B
    ram[i] = 0xCB;
    i++;
    ram[i] = 0x00;
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

