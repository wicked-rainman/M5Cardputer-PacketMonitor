#include <SD.h>
#include <FS.h>
#include "Defines.h"
#include "ScreenPrint.h"
#include "StoreStruct.h"
void DumpNetworks();
void DumpDevices();
void DumpFile(char *, bool);
char *LookupOui(char *);