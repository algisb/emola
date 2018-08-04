#ifndef GBDEVICE_H_
#define GBDEVICE_H_
struct GBDevice;
typedef struct GBDevice GBDevice;


int createGBDevice(GBDevice ** _dev);
int destroyGBDevice(GBDevice ** _dev);
int run(GBDevice ** _dev);

#endif
