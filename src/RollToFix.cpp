#include "RollToFix.h"
void RollToFix(void *SummatElse) {
//
// Rolling MAC addresses are sometimes related to a fixed MAC
// (for example, with some commercial APs).
// Such rolling MAC addresses might turn up before or after a fixed 
// associated MAC. This causes a boundary clash when making the
// association. This routine fixes that issue whenever it is called, but
// only when a fixed association MAC has already been stored.
// For any associated rolling mac, status is changed from 'r' to
// 'e' so that the association can be detected in follow on
// processing.
//
extern SemaphoreHandle_t xShmem;
extern int StoreLastUsed;
static char msg[160];
int ctr;
int innerctr;
static int LenSsid;
static char Mac[20];
static int OldNetworkCount;
  while(true) {
    if(xSemaphoreTake(xShmem,15000)) {
      if(OldNetworkCount != StoreLastUsed) {
        OldNetworkCount=StoreLastUsed;
        DrawCircle(230,95,5,TFT_RED);
        for(ctr=0;ctr<MAX_SSID_STORE_SIZE;ctr++){
          if((storeArray[ctr].rolling == 'R') && (storeArray[ctr].mode =='A')) {
            for(innerctr=0;innerctr<MAX_SSID_STORE_SIZE;innerctr++) {
              if((storeArray[innerctr].rolling =='F') && (storeArray[innerctr].mode =='A')) {
                if(!memcmp(storeArray[ctr].MacAddress+1, storeArray[innerctr].MacAddress+1,4)) {
                  LenSsid=strlen(storeArray[innerctr].Ssid);
                  strncpy(storeArray[ctr].Ssid_Assoc, storeArray[innerctr].Ssid,LenSsid);
                  storeArray[ctr].Ssid_Assoc[LenSsid] = 0x0;
                  storeArray[ctr].mode = 'E';
                  snprintf(Mac,20,"%02X%02X%02X%02X%02X%02X",
                    storeArray[ctr].MacAddress[0],storeArray[ctr].MacAddress[1],storeArray[ctr].MacAddress[2],
                    storeArray[ctr].MacAddress[3],storeArray[ctr].MacAddress[4],storeArray[ctr].MacAddress[5]);
                  if(GENERATE_SERIAL_OUTPUT) {
                    snprintf(msg,80,"%03d,%c,%s,%c,%d,%s,%s,%s",
                      ctr,
                      storeArray[ctr].mode,
                      Mac,
                      storeArray[ctr].rolling,
                      storeArray[ctr].Rssi,
                      storeArray[ctr].Ssid,
                      storeArray[ctr].Ssid_Assoc,
                      storeArray[ctr].fix);
                    USBSerial.printf("%s\n",msg);
                  }
                  ScreenPrint(storeArray[ctr].Ssid,32,1,6,TFT_GREEN,TFT_BLACK);
                  ScreenPrint(storeArray[ctr].Ssid_Assoc,32,3,6,TFT_YELLOW,TFT_BLACK);;
                  ScreenPrint(Mac,18,5,6,TFT_GREEN,TFT_BLACK);
                  memset(msg,' ',19);
                  ScreenPrint(msg,18,7,6,TFT_GREEN,TFT_BLACK);
                  ScreenPrint(msg,1,5,31,TFT_GREEN,TFT_BLACK);
                  ScreenPrint(msg,1,7,31,TFT_GREEN,TFT_BLACK);
                  snprintf(msg,4,"%03d",ctr);
                  ScreenPrint(msg,3,9,2,TFT_GREEN,TFT_BLACK);
                  snprintf(msg,4,"%d",storeArray[ctr].Rssi);
                  ScreenPrint(msg,4,9,12,TFT_GREEN,TFT_BLACK);
                  snprintf(msg,2,"%c",storeArray[ctr].mode);
                  ScreenPrint(msg,1,9,21,TFT_YELLOW,TFT_BLACK);
                  vTaskDelay(60);
                }
              }
            }
          }
        }
      }
      DrawCircle(230,95,5,TFT_DARKGREY);
      xSemaphoreGive(xShmem);
    }
    vTaskDelay(ROLL_FIX_INTERVAL);
  } 
}