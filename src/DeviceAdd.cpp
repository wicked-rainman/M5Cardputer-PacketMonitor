#include "DeviceAdd.h"
//-----------------------------------------------------------------------
// Function: Device add
// Purpose: Services the queue of management packet headers generated by
// the WiFi packet handler.
// 1. Reads new MAC address pairs from the queue (sender and receiver)
// 2. Test to see if the addresses are rolling and sets appropriate flag
// 3. Look in a rolling cache store to see if the mac pairs have been seen before
// 4. Add new found macs to the cache, date and lat/lon stamped.
//-----------------------------------------------------------------------

void DeviceAdd(void *abc) {

    extern SemaphoreHandle_t xShmem;                                                    //Shared memory lock
    extern QueueHandle_t PacketQueue;                                                   //Wifi packet handler incomming records
    extern char DeviceTable[][160];                                                     //Fully defined in main.h
    extern int DeviceCount;                                                             //Count of mac pairs currently in cache
    extern Store storeArray[MAX_SSID_STORE_SIZE+1];                                     //Storage for devices seen so far (Rolling)
    uint8_t NewMacs[12];                                                                //Two six byte macs
    uint8_t Mac1[6];                                                                    //Sender mac
    uint8_t Mac2[6];                                                                    //Receiver mac
    static char Mac1Str[19];                                                            //Mac 1 as a hex string
    static char Mac2Str[19];                                                            //Mac 2 as a hex string
    static char AssocSsid[MAX_SSID_LENGTH];                                             //Rolling mac's broadcast ssid string
    static char ssid1[MAX_SSID_LENGTH];                                                 //Fixed mac's broadcast ssid string
    char Mac2R;                                                                         //Mac2 rolling or fixed ('R' or 'F')
    char Mac1R;                                                                         //Mac1 rolling or fixed ('R' or 'F')
    char GpsStr[25];                                                                    //GPS Lat/Lon time string
    char GpsTime[9];                                                                    //GPS Time string
    extern char GpsFix[];                                                               //'A' or not 'A'
    extern bool GpsLock;                                                                //Good fix established=true
    extern ESP32Time rtc;                                                               //Inaccurate internal RTC
    static char msg[80];                                                                //Temp space for values I/O
    int RetVal;                                                                         //Search result, -1 or n.

    while(true) {
        if( xQueueReceive(PacketQueue, &NewMacs, (TickType_t) 5)) {                     //If there is a packet in the queue
            DrawCircle(230,65,5,TFT_ORANGE);                                            //Turn on "device found" indicator
            memcpy(Mac1,NewMacs,6);                                                     //Copy in mac address 1
            memcpy(Mac2,NewMacs+6,6);                                                   //Copy in mac address 2
            RollingMac(Mac1[0]) ? Mac1R='R' : Mac1R='F';                                //Test mac1 rolling or fixed
            RollingMac(Mac2[0]) ? Mac2R='R' : Mac2R='F';                                //Test mac 2 rolling or fixed
            snprintf(Mac1Str,18,"%02X%02X%02X%02X%02X%02X",                             //Make a hex string of mac 1
                Mac1[0],Mac1[1],Mac1[2],Mac1[3],Mac1[4],Mac1[5]);
            snprintf(Mac2Str,18,"%02X%02X%02X%02X%02X%02X",                             //Make a hex string of mac 2
                Mac2[0],Mac2[1],Mac2[2],Mac2[3],Mac2[4],Mac2[5]);
            memcpy(GpsStr,"xxxx.xxxxxN,xxxxx.xxxxxW\0",25);                             //Lost lock LAT/LON
            memcpy(GpsTime,rtc.getTime().c_str(),9);                                    //Store TAI in case GPS lock lost
            memset(AssocSsid,0,MAX_SSID_LENGTH);                                        //Default null Associated ssid.
            strcpy(ssid1,"-");                                                          //Default, empty ssid.
            if(xSemaphoreTake(xShmem,500)) {                                            //If the semaphore can be taken
                RetVal = StoreFind(Mac2);                                               //Look up the receiver mac to see if there's an ssid.
                if(RetVal>=0) {                                                         //Mac2 is found in the store
                    memcpy(ssid1,storeArray[RetVal].Ssid,MAX_SSID_LENGTH);              //Copy in ssid
                    memcpy(AssocSsid,storeArray[RetVal].Ssid_Assoc,MAX_SSID_LENGTH);    //Copy in Associated ssid
                }
                if(GpsLock) {                                                           //If there is a current GPS lock
                    memcpy(GpsStr,GpsFix+9,25);                                         //Copy in the GPS Fix (lat/lon)
                    GpsStr[25]=0x0;             
                    memcpy(GpsTime,GpsFix,8);                                           //Copy in the TAI.
                    GpsTime[8]=0x0;         
                }
                xSemaphoreGive(xShmem);                                                 //Release the semaphore
            }
            ScreenPrint(ssid1,MAX_SSID_LENGTH,1,6,TFT_GREEN,TFT_BLACK);                 //Output values on screen as required
            ScreenPrint(AssocSsid,MAX_SSID_LENGTH,3,6,TFT_GREEN,TFT_BLACK);
            ScreenPrint(Mac1Str,18,5,6,TFT_GREEN,TFT_BLACK);
            ScreenPrint(Mac2Str,18,7,6,TFT_GREEN,TFT_BLACK); 
            snprintf(msg,2,"%c",Mac1R);
            ScreenPrint(msg,2,5,31,TFT_GREEN,TFT_BLACK);
            snprintf(msg,2,"%c",Mac2R);
            ScreenPrint(msg,2,7,31,TFT_GREEN,TFT_BLACK);
            snprintf(msg,4,"%03d",DeviceCount);
            ScreenPrint(msg,3,9,2,TFT_GREEN,TFT_BLACK);
            ScreenPrint((char *) "-00 ",3,9,12,TFT_GREEN,TFT_BLACK);
            ScreenPrint((char *) "D ",1,9,21,TFT_GREEN,TFT_BLACK);
            ScreenPrint(GpsStr,25,11,6,TFT_GREEN,TFT_BLACK);
                                                                                        //Now prepare data for storing
            if((Mac2R=='R') && (AssocSsid[0]!=0)) {                                     //Rolling mac 2 and Assoc ssid present 
                snprintf(DeviceTable[DeviceCount],160,"%03d,D,%s,%c,%s,%c,%s(%s),%s,%s\n",
                DeviceCount,Mac1Str,Mac1R,Mac2Str,Mac2R,ssid1,AssocSsid,GpsTime,GpsStr);
            }
            else {
                snprintf(DeviceTable[DeviceCount],160,"%03d,D,%s,%c,%s,%c,%s,%s,%s\n", //No assoc'd ssid
                DeviceCount,Mac1Str,Mac1R,Mac2Str,Mac2R,ssid1,GpsTime,GpsStr);
            }
            DrawRect(5,115,DeviceCount,4,TFT_RED);                                      //Increment devices bar chart
            if(GENERATE_SERIAL_OUTPUT) USBSerial.printf("%s",DeviceTable[DeviceCount]); //Serial output each new entry
            DeviceCount++;
            if(DeviceCount == MAX_DEVICE_TABLE_SIZE) {                                  //If table is full
                DumpDevices();                                                          //Append the cache entries to /devices.txt 
                DeviceCount=0;                                                          //Reset DeviceCount index to cache
                DrawRect(5,115,MAX_DEVICE_TABLE_SIZE,4,TFT_BLACK);                      //Black out the device bargraph
            }
            DrawCircle(230,65,5,TFT_DARKGREY);                                          //Turn off "device found" indicator
        }
        vTaskDelay(60);
    }
}