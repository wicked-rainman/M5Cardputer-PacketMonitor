#include <M5Cardputer.h>
#include <SPI.h>
#include <WiFi.h>
#include <SD.h>
#include <ESP32Time.h>
#include "Defines.h"
#include "Prototypes.h"
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


Store storeArray[MAX_SSID_STORE_SIZE+1];
char DeviceTable[MAX_DEVICE_TABLE_SIZE][DEVICE_TABLE_RECSIZE];
int StoreLastUsed=0;
int DeviceCount=0;
int TaskCount=0;
char GpsFix[40];
bool GpsLock=false;
ESP32Time rtc(0);
