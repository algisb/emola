#ifndef ALU_H_
#define ALU_H_
#include <stdint.h>

struct CPU;
typedef struct CPU CPU;

void add(CPU * _cpu,  uint8_t * _n );
void adc(CPU * _cpu,  uint8_t * _n );
void sub(CPU * _cpu,  uint8_t * _n );
void sbc(CPU * _cpu,  uint8_t * _n );
void and(CPU * _cpu,  uint8_t * _n );
void xor(CPU * _cpu,  uint8_t * _n );
void or(CPU * _cpu,  uint8_t * _n );
void cp(CPU * _cpu,  uint8_t * _n );

void addHLuint16(CPU * _cpu,  uint16_t * _n );
void addSPint8(CPU * _cpu,  int8_t * _n );
#endif
