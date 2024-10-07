#include "DeviceAdd.h"
;
void DeviceAdd(void *abc) {
    extern SemaphoreHandle_t xShmem;
    extern QueueHandle_t PacketQueue;
    extern uint8_t NewMacs[12];
    extern char DeviceTable[][160];
    extern int DeviceCount;
    int RetVal;
    static uint8_t Mac1[6];
    static uint8_t Mac2[6];
    static char Mac1Str[19];
    static char Mac2Str[19];
    static char ssid[35];
    static char Mac1R,Mac2R;
    static char assocssid[35];
    static char GpsStr[25];
    static char GpsTime[9];
    extern char GpsFix[];
    extern bool GpsLock;
    extern ESP32Time rtc;

    static char msg[80];
    while(true) {
        if( xQueueReceive(PacketQueue, &NewMacs, (TickType_t) 5)) {
            memcpy(Mac1,NewMacs,6);
            memcpy(Mac2,NewMacs+6,6);
            DrawCircle(230,65,5,TFT_ORANGE);
            RollingMac(Mac1[0]) ? Mac1R='R' : Mac1R='F';
            RollingMac(Mac2[0]) ? Mac2R='R' : Mac2R='F';
            memset(ssid,0,35);
            memset(assocssid,0,35);
            if(xSemaphoreTake(xShmem,500)) { //If the semaphore can be taken
                RetVal = StoreFind(Mac2); // Look up the receiver mac to see if there's an ssid.
                if(RetVal>=0) {
                    memcpy(ssid,storeArray[RetVal].Ssid,MAX_SSID_LENGTH);
                    memcpy(assocssid,storeArray[RetVal].Ssid_Assoc,MAX_SSID_LENGTH);
                }
                else {
                    memset(ssid,0,35);
                    memset(assocssid,0,35);
                }
                if(GpsLock) {
                    memcpy(GpsStr,GpsFix+9,25);
                    GpsStr[25]=0x0;
                    memcpy(GpsTime,GpsFix,8);
                    GpsTime[8]=0x0;
                }
                else {
                    memcpy(GpsStr,"xxxx.xxxxxN xxxxx.xxxxxW\0",25);
                    memcpy(GpsTime,rtc.getTime().c_str(),9);
                }
                xSemaphoreGive(xShmem);
            }
            snprintf(Mac1Str,18,"%02X%02X%02X%02X%02X%02X", NewMacs[0],NewMacs[1],NewMacs[2],NewMacs[3],NewMacs[4],NewMacs[5]);
            snprintf(Mac2Str,18,"%02X%02X%02X%02X%02X%02X", NewMacs[6],NewMacs[7],NewMacs[8],NewMacs[9],NewMacs[10],NewMacs[11]);

            ScreenPrint(ssid,32,1,6,TFT_GREEN,TFT_BLACK);
            ScreenPrint(assocssid,32,3,6,TFT_GREEN,TFT_BLACK);
            ScreenPrint(Mac1Str,18,5,6,TFT_GREEN,TFT_BLACK);
            ScreenPrint(Mac2Str,18,7,6,TFT_GREEN,TFT_BLACK); 
            snprintf(msg,2,"%c",Mac1R);
            ScreenPrint(msg,2,5,31,TFT_GREEN,TFT_BLACK);
            snprintf(msg,2,"%c",Mac2R);
            ScreenPrint(msg,2,7,31,TFT_GREEN,TFT_BLACK);
            snprintf(msg,4,"%03d",DeviceCount);
            ScreenPrint(msg,3,9,2,TFT_GREEN,TFT_BLACK);
            ScreenPrint("-00 ",3,9,12,TFT_GREEN,TFT_BLACK);
            ScreenPrint("D ",1,9,21,TFT_GREEN,TFT_BLACK);
            ScreenPrint(GpsStr,25,11,6,TFT_GREEN,TFT_BLACK);
            ScreenPrint(GpsTime,9,13,6,TFT_GREEN,TFT_BLACK);
            snprintf(DeviceTable[DeviceCount],110,"%03d D %s %c %s %c \"%s\" \"%s\" %s %s\n",DeviceCount,
            Mac1Str,Mac1R,Mac2Str,Mac2R, ssid,assocssid,GpsTime,GpsStr);
            DrawRect(5,115,DeviceCount*2,4,TFT_RED);
            #ifdef GENERATE_SERIAL_OUTPUT
                USBSerial.printf("%s",DeviceTable[DeviceCount]);
            #endif
            DeviceCount++;
            //Table is full, so dump it's contents to SD and blank out the Device bar graph.
            //Reset DeviceCount back to zero.
            if(DeviceCount == MAX_DEVICE_TABLE_SIZE) {
                DumpDevices();
                DeviceCount=0;
                DrawRect(5,115,MAX_DEVICE_TABLE_SIZE*2,4,TFT_BLACK);
            }
            DrawCircle(230,65,5,TFT_DARKGREY);
        }
        vTaskDelay(60);
    }
}