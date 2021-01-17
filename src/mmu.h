#ifndef MMU_H_
#define MMU_H_
#include <stdint.h>

void createRAM(uint8_t** _ram);
uint64_t loadBin(uint8_t* _ram, uint16_t _loc, char* _path);

#endif
