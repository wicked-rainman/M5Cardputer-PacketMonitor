#include "NetworkScan.h"
void NetworkScan(void *Summat) {
  int16_t NetworkCount;
  int16_t ctr;
  extern SemaphoreHandle_t xShmem;
  while(true) {
    if(xSemaphoreTake(xShmem,5000)) {
      NetworkCount = WiFi.scanNetworks(false,true,true);
      for (ctr = 0; ctr < NetworkCount; ctr++) { 
        StorePut(WiFi.BSSID(ctr), (char *) WiFi.SSID(ctr).c_str(),WiFi.RSSI(ctr)); 
      }
      xSemaphoreGive(xShmem);
    }
    vTaskDelay(SCAN_INTERVAL);
  }
}