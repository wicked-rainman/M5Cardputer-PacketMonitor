#include <M5Cardputer.h>
#include <SD.h>
#include <FS.h>
#include "Defines.h"
#include "StoreStruct.h"
void DumpNetworks();
void DumpDevices();
void DumpFile(char *, bool);
extern char *LookupOui(char *TargetMac);
