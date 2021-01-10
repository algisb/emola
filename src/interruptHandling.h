#ifndef INTERRUPT_HANDLING_H_
#define INTERRUPT_HANDLING_H_
#include <stdint.h>

struct CPU;
typedef struct CPU CPU;

void handleInterrupts(CPU* _cpu, uint8_t* _memory);


#endif
