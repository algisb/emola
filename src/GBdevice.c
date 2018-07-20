#include "GBdevice.h"
#include <stdlib.h>
#include <stdio.h>
#include "cpu.h"
struct GBDevice
{
    int on;
    CPU * cpu;
};

int createGBDevice(GBDevice * _dev)
{
    _dev = (GBDevice*)malloc(sizeof(GBDevice));
    on = 1;
    createCPU(_dev->cpu);
    
    return 0;
}

int destroyGBDevice(GBDevice * _dev)
{
    destroyCPU(_dev->cpu);
    free(_dev);
    return 0;
}


int run(GBDevice * _dev)
{

    return 0;
}
