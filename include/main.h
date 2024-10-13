#include <M5Cardputer.h>
#include <SPI.h>
#include <WiFi.h>
#include <SD.h>
#include <ESP32Time.h>
#include "Defines.h"
#include "StoreStruct.h"
#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include "esp_system.h"
#include "esp_event.h"
//-------------------------------------------
static wifi_country_t wifi_country = {.cc="GB", .schan = 1, .nchan = 13}; 

SemaphoreHandle_t xScreen = NULL;
SemaphoreHandle_t xShmem = NULL;
QueueHandle_t PacketQueue;
TaskHandle_t tasks[10];
extern void ScreenPrint(char *, uint8_t,uint8_t, uint8_t,uint16_t,uint16_t);
extern void DrawCircle(int32_t, int32_t, int32_t,int);
extern void NetworkScan(void *);
extern void RollToFix(void *);
extern void WiFiPacketHandler(void*, wifi_promiscuous_pkt_type_t type);
extern void ChannelHop(void *);
extern void DeviceAdd(void *);
extern void KeyboardBranch(void *);
extern void BatteryStatus(void *);
extern void GpsReader(void *);

Store storeArray[MAX_SSID_STORE_SIZE+1];
char DeviceTable[MAX_DEVICE_TABLE_SIZE][DEVICE_TABLE_RECSIZE];
int StoreLastUsed=0;
int DeviceCount=0;
int TaskCount=0;
char GpsFix[40];
bool GpsLock=false;
ESP32Time rtc(0);
