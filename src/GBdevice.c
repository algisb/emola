#include "GBdevice.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "cpu.h"
#include "defs.h"
struct GBDevice
{
    int on;
    CPU * cpu;
    unsigned char * ram;
    double timePrev;
    unsigned long long ticks;
};

int createGBDevice(GBDevice ** _dev)
{
    *_dev = (GBDevice*)malloc(sizeof(GBDevice));
    (*_dev)->on = 1;
    createCPU(&(*_dev)->cpu);
    (*_dev)->ram = (unsigned char*)malloc(RAM_SIZE);
    (*_dev)->timePrev = 0.0f;
    (*_dev)->ticks = 0;
    
    return 0;
}

int destroyGBDevice(GBDevice ** _dev)
{
    destroyCPU(&(*_dev)->cpu);
    free(*_dev);
    return 0;
}


int run(GBDevice ** _dev)
{
    while((*_dev)->on)//each tick
    {
        fetchInst(&(*_dev)->cpu, &(*_dev)->ram);
        decodeExecInst(&(*_dev)->cpu, &(*_dev)->ram);
    }
    printf("GBdevice turning off \n");
    return 0;
}
