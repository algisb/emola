#include "mmu.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "defs.h"
#include "debug.h"

void createRAM(uint8_t** _ram)
{
    *_ram = (uint8_t*)calloc(GB_ADDRESS_SPACE_SIZE, 1);
    uint8_t * ram = *_ram;
    uint16_t * tmp_uint16; 
    int i = 0;
    i = loadBin(ram, 0x00, "DMG_ROM.bin");
    i++;
//     //NOP
//     ram[i] = 0x00;
//     i++;
//     //LD (**) SP 
//     ram[i] = 0x08;
//     i++;
//     tmp_uint16 = (uint16_t*)(ram + i);
//     *tmp_uint16 = 69;
//     i+=2;
//     
//     //LD HL dd
//     ram[i] = 0x21;
//     i++;
//     tmp_uint16 = (uint16_t*)(ram + i);
//     *tmp_uint16 = 799;
//     i+=2;
//     
//     //LD (HL+), A
//     ram[i] = 0x22;
//     i++;
//     
//     //LD A, (BC)
//     ram[i] = 0x0A;
//     i++;
//     
//     //inc D
//     ram[i] = 0x14;
//     i++;
// 
//     //inc BC
//     ram[i] = 0x03;
//     i++;    
//     
//     //LD B, dd
//     ram[i] = 0x06;
//     i++;
//     ram[i] = 111;
//     i++;
//     
//     //RLC B
//     ram[i] = 0xCB;
//     i++;
//     ram[i] = 0x00;
//     i++;
    
    
    //HALT
    ram[i] = 0x76;
//     
    PRINT_DEBUG("program in memory:\n");
    for(int j = 0; j<i; j++)
        PRINT_DEBUG("0x%02x ",ram[j]);
    PRINT_DEBUG("\n");
    PRINT_DEBUG("-------------------------------\n");
    

}
uint64_t loadBin(uint8_t* _ram, uint16_t _loc, char* _path)
{
    FILE * pFile;
    long lSize;
    char * buffer;
    size_t result;

    pFile = fopen (_path, "rb");
    if (pFile==NULL) {fputs ("File error",stderr); exit (1);}

    // obtain file size:
    fseek (pFile , 0 , SEEK_END);
    lSize = ftell (pFile);
    rewind (pFile);

    // allocate memory to contain the whole file:
    buffer = (char*) malloc (sizeof(char)*lSize);
    if (buffer == NULL) {fputs ("Memory error",stderr); exit (2);}

    // copy the file into the buffer:
    result = fread (buffer,1,lSize,pFile);
    if (result != lSize) {fputs ("Reading error",stderr); exit (3);}
    PRINT_DEBUG("Loaded binary size %lu bytes\n", lSize);
    
    memcpy(&_ram[_loc], buffer, lSize);
    
    // terminate
    fclose (pFile);
    free (buffer);
    return lSize;
}

