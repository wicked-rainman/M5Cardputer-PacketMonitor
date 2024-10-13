#include <M5Cardputer.h>
#include <ESP32Time.h>
#include "Defines.h"
#include "StoreStruct.h"
bool StorePut(uint8_t *, char *, int32_t);
int StoreFind(uint8_t *);
bool RollingMac(uint8_t);
