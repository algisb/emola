#ifndef GBDEVICE_H_
#define GBDEVICE_H_
struct GBDevice;
typedef struct GBDevice GBDevice;


void createGBDevice(GBDevice ** _dev);
void destroyGBDevice(GBDevice ** _dev);
void run(GBDevice * _dev);

#endif
