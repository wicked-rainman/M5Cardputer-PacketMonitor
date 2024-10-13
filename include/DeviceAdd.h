#include <M5Cardputer.h>
#include <ESP32Time.h>
#include "Defines.h"
#include "StoreStruct.h"
void DeviceAdd(void *);
extern void DumpDevices();
extern void ScreenPrint(char *, uint8_t,uint8_t, uint8_t,uint16_t,uint16_t);
extern int StoreFind(uint8_t *);
extern bool RollingMac(uint8_t);
extern void DrawCircle(int32_t, int32_t, int32_t,int);
extern void DrawRect(int32_t,int32_t,int32_t,int32_t,int);