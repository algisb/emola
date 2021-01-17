#include "GBdevice.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "cpu.h"
#include "cpu_t.h"
#include "defs.h"
#include "mmu.h"

void createGBDevice(GBDevice ** _dev)
{
    GBDevice * dev;
    //dynamic memory alloc
    dev = (GBDevice*)malloc(sizeof(GBDevice));
    runTestsCPU();
    createCPU(&dev->cpu);
    createRAM(&dev->ram);
    mapRegisters(dev->cpu, dev->ram);
    
    
    dev->on = 1;
    *_dev = dev;

}

void destroyGBDevice(GBDevice ** _dev)
{
    //dynamic memory dealloc
    free(&(*_dev)->ram);
    destroyCPU(&(*_dev)->cpu);
    free(*_dev);
}


void run(GBDevice *_dev)
{
    //sim runs at 60 fps
    while(_dev->cpu->cycles < GB_CYCLES_PER_FRAME)
    {
        feDeExInst(_dev->cpu, _dev->ram);
    }
    _dev->cpu->cycles = 0;
}
