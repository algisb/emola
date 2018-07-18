#include "GBdevice.h"
#include <stdlib.h>
#include <stdio.h>


int createGBDevice(GBDevice * _dev)
{
    _dev = (GBDevice*)malloc(sizeof(GBDevice));
    return 0;
}

int destroyGBDevice(GBDevice * _dev)
{
    return 0;
}


int run(GBDevice * _dev)
{
    free(_dev);
    return 0;
}
