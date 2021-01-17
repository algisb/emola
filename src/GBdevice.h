#ifndef GBDEVICE_H_
#define GBDEVICE_H_

struct CPU;
typedef struct CPU CPU;
struct GBDevice
{
    int on;
    CPU * cpu;
    unsigned char * ram;
};
typedef struct GBDevice GBDevice;

void createGBDevice(GBDevice ** _dev);
void destroyGBDevice(GBDevice ** _dev);
void run(GBDevice * _dev);

#endif
