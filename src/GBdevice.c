#include "GBdevice.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "cpu.h"
#include "defs.h"
#include "mmu.h"
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
    GBDevice * dev;
    //dynamic memory alloc
    dev = (GBDevice*)malloc(sizeof(GBDevice));
    createCPU(&dev->cpu);
    createRAM(&dev->ram);
    
    
    dev->on = 1;
    dev->timePrev = 0.0f;
    dev->ticks = 0;
    *_dev = dev;
    
    return 0;
}

int destroyGBDevice(GBDevice ** _dev)
{
    //dynamic memory dealloc
    free(&(*_dev)->ram);
    destroyCPU(&(*_dev)->cpu);
    free(*_dev);
    return 0;
}


int run(GBDevice *_dev)
{
    while(_dev->on)//each tick
    {
        fetchInst(_dev->cpu, _dev->ram);
        decodeExecInst(_dev->cpu, _dev->ram);
    }
    printf("GBdevice turning off \n");              
    return 0;
}
