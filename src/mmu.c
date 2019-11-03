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
//     //instruction 1(LD B 69)
//     ram[i] = 0x06;
//     i++;
//     ram[i] = 69;
//     i++;
//     //instruction 2(LD H 69)
//     ram[i] = 0x26;
//     i++;
//     ram[i] = 69;
//     i++;
//     
//     //instruction 3(LD HL **)
//     ram[i] = 0x21;
//     i++;
//     short * tmp = (ram + i);
//     *tmp = 1420;
//     i+=2;
//     
//     //instruction 4 (LD (HL) *)
//     ram[i] = 0x36;
//     i++;
//     ram[i] = 47;
//     i++;
//     
//     
//     //instruction 3(LD HL **)
//     ram[i] = 0x31;
//     i++;
//     tmp = (ram + i);
//     *tmp = 1793;
//     i+=2;
    
    
    
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

