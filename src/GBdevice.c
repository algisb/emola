#include "GBdevice.h"
#include <stdlib.h>
#include <stdio.h>
#include "cpu.h"
#include "defs.h"
struct GBDevice
{
    int on;
    CPU * cpu;
    unsigned char * ram;
};

int createGBDevice(GBDevice ** _dev)
{
    *_dev = (GBDevice*)malloc(sizeof(GBDevice));
    (*_dev)->on = 1;
    createCPU(&(*_dev)->cpu);
    (*_dev)->ram = (unsigned char*)malloc(RAM_SIZE);
    
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

    printf("kek\n");
    return 0;
}
