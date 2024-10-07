#include <ESP32Time.h>
#include "Defines.h"
#include "ScreenPrint.h"
#include "StoreStruct.h"
#include "DumpStoreToSd.h"
bool StorePut(uint8_t *, char *, int32_t);
int StoreFind(uint8_t *);
bool RollingMac(uint8_t);
extern void CharReplace(char *, char, char);
