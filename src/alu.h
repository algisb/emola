#ifndef ALU_H_
#define ALU_H_
#include <stdint.h>

struct CPU;
typedef struct CPU CPU;

void add(CPU * _cpu,  uint8_t * _n );
void adc(CPU * _cpu,  uint8_t * _n );
void sub(CPU * _cpu,  uint8_t * _n );
void sbc(CPU * _cpu,  uint8_t * _n );
#endif
