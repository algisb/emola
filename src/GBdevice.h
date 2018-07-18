#ifndef GBDEVICE_H_
#define GBDEVICE_H_
#include "cpu.h"
typedef struct
{
    CPU cpu;
}GBDevice;
int createGBDevice(GBDevice * _dev);
int destroyGBDevice(GBDevice * _dev);
int run(GBDevice * _dev);

#endif
