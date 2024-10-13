#ifndef PROTOTYPES
    #define PROTOTYPES
    void BatteryStatus(void *);
    void ChannelHop(void *);
    void CharReplace(char *, char, char);
    void DeviceAdd(void *);
    void DrawCircle(int32_t, int32_t, int32_t,int);
    void DrawRect(int32_t,int32_t,int32_t,int32_t,int);
    void DumpDevices();
    void DumpFile(char *, bool);
    void DumpNetworks();
    void GpsReader(void *);
    void KeyboardBranch(void *);
    char *LookupOui(char *TargetMac);
    void NetworkScan(void *);
    bool RollingMac(uint8_t);
    void RollToFix(void *);
    void ScreenPrint(char *, uint8_t,uint8_t, uint8_t,uint16_t,uint16_t);
    int StoreFind(uint8_t *);
    bool StorePut(uint8_t *, char *, int32_t);
    void WiFiPacketHandler(void*, wifi_promiscuous_pkt_type_t type);
#endif