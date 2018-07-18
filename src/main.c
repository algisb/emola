#include <stdlib.h>
#include <stdio.h>
#include "GBdevice.h"

int main(int argc, char ** argv)
{
	GBDevice * device = NULL;
    
    createGBDevice(device);
    run(device);
    destroyGBDevice(device);

	return 0;
}
