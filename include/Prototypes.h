#ifndef PROTOTYPES
#define PROTOTYPES
void ScreenPrint(char *, uint8_t,uint8_t, uint8_t,uint16_t,uint16_t);
void DrawRect(int32_t,int32_t,int32_t,int32_t,int);
void DrawCircle(int32_t, int32_t, int32_t,int);
void ChannelHop(void *);
void DeviceAdd(void *);
void DumpDevices();
int StoreFind(uint8_t *);
bool RollingMac(uint8_t);
void RollToFix(void *);
void DumpNetworks();
void DumpFile(char *, bool);
void DumpDevices();
char *LookupOui(char *TargetMac);
void BatteryStatus(void *);
void GpsReader(void *);
void NetworkScan(void *);
void WiFiPacketHandler(void*, wifi_promiscuous_pkt_type_t type);
void KeyboardBranch(void *);
bool StorePut(uint8_t *, char *, int32_t);
void CharReplace(char *, char, char);
#endif