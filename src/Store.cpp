#include "Store.h"
//---------------------------------------------------------
// Functions StorePut and StoreFind
// Purpose: Maintains a rolling cache of MAC addresses
//---------------------------------------------------------
bool StorePut(uint8_t index[], char *val, int32_t rssival) {
    extern void ScreenPrint(char *, uint8_t,uint8_t, uint8_t,uint16_t,uint16_t);
    extern void DrawCircle(int32_t, int32_t, int32_t,int);
    extern void DrawRect(int32_t,int32_t,int32_t,int32_t,int);
    extern void DumpNetworks();
    static char msg[80];
    static char Mac[20];
    static int ValLen;
    extern int StoreLastUsed;
    extern bool GpsLock;
    extern char GpsFix[];
    static char GpsStr[25];
    static char GpsTime[9];
    extern ESP32Time rtc;
    extern void CharReplace(char *, char, char);

    if(StoreFind(index)<0) {                                           
        DrawCircle(230,80,5,TFT_GREEN);
        if(StoreLastUsed==MAX_SSID_STORE_SIZE) {
            DumpNetworks();
            StoreLastUsed=0;
            DrawRect(5,124,MAX_SSID_STORE_SIZE,5,TFT_BLACK);
        }
        ValLen=strlen(val);
        memcpy(storeArray[StoreLastUsed].MacAddress, index, (size_t) 6);
        snprintf(Mac,20,"%02X%02X%02X%02X%02X%02X",index[0],index[1],index[2],index[3],index[4],index[5]);
        CharReplace(val,',','-');
        CharReplace(val,' ','_');
        if(ValLen==0) { 
            memcpy(storeArray[StoreLastUsed].Ssid,"Hidden",7);
            storeArray[StoreLastUsed].Ssid[6]=0x0;
        }
        else {
            memcpy(storeArray[StoreLastUsed].Ssid, val,ValLen);
            storeArray[StoreLastUsed].Ssid[ValLen]=0x0;
        }
        storeArray[StoreLastUsed].Rssi = rssival;
        RollingMac(index[0]) ? (storeArray[StoreLastUsed].rolling = 'R'):(storeArray[StoreLastUsed].rolling='F');
        storeArray[StoreLastUsed].mode='A';
        memset(storeArray[StoreLastUsed].Ssid_Assoc,0,MAX_SSID_LEN);   
        if(GpsLock) {
            memcpy(storeArray[StoreLastUsed].fix,GpsFix,40);
            memcpy(GpsStr,GpsFix+9,25);
            GpsStr[25]=0x0;
            memcpy(GpsTime,GpsFix,8);
            GpsTime[8]=0x0;
        }
        else { //No gps lock, so use rtc value for time
            memcpy(GpsStr,"xxxx.xxxxxN,xxxxx.xxxxxW\0",25);
            memcpy(GpsTime,rtc.getTime().c_str(),9);
            memcpy(storeArray[StoreLastUsed].fix,GpsTime,8);
            storeArray[StoreLastUsed].fix[8]=' ';
            memcpy(storeArray[StoreLastUsed].fix+9,GpsStr,25);;
        }

        if(GENERATE_SERIAL_OUTPUT) {
            snprintf(msg,80,"%03d,%c,%s,%c,%d,%s,%s",
                StoreLastUsed,
                storeArray[StoreLastUsed].mode,
                Mac,
                storeArray[StoreLastUsed].rolling,
                storeArray[StoreLastUsed].Rssi,
                storeArray[StoreLastUsed].Ssid,storeArray[StoreLastUsed].fix);
                USBSerial.printf("%s\n",msg);
        }
        ScreenPrint(storeArray[StoreLastUsed].Ssid,32,1,6,TFT_GREEN,TFT_BLACK);
        memset(msg,' ',80);
        ScreenPrint(msg,32,3,6,TFT_BLACK,TFT_BLACK);            //Blank Ssid assoc
        ScreenPrint(Mac,18,5,6,TFT_GREEN,TFT_BLACK);            //Print Receiver Mac
        ScreenPrint(msg,18,7,6,TFT_BLACK,TFT_BLACK);            //Blank out sender mac                    
        snprintf(msg,4,"%03d",StoreLastUsed); 
        ScreenPrint(msg,3,9,2,TFT_GREEN,TFT_BLACK);             //Last SSID table number
        snprintf(msg,4,"%d",storeArray[StoreLastUsed].Rssi);
        ScreenPrint(msg,4,9,12,TFT_GREEN,TFT_BLACK);            //RSSI
        snprintf(msg,2,"%c",storeArray[StoreLastUsed].mode);
        ScreenPrint(msg,1,9,21,TFT_GREEN,TFT_BLACK);            //Mode
        snprintf(msg,2,"%c",storeArray[StoreLastUsed].rolling);
        ScreenPrint(msg,1,5,31,TFT_GREEN,TFT_BLACK);            //Rolling flag for recv
        ScreenPrint((char *) " ",1,7,31,TFT_BLACK,TFT_BLACK);   //Rolling flag for sndr
        ScreenPrint(GpsStr,25,11,6,TFT_GREEN,TFT_BLACK);
        ScreenPrint(GpsTime,9,13,6,TFT_GREEN,TFT_BLACK);
        StoreLastUsed++;
        DrawCircle(230,80,5,TFT_DARKGREY);
        DrawRect(5,124,StoreLastUsed,5,TFT_GREEN);
        return true;
    }
    return false;
}
//---------------------------------------------------------
int StoreFind(uint8_t index[]) {
    uint8_t storeCtr;
    for(storeCtr=0;storeCtr<MAX_SSID_STORE_SIZE;storeCtr++) {
        if (!memcmp(storeArray[storeCtr].MacAddress, index,6)) return storeCtr;
    }
    return(-1);
}
//---------------------------------------------------------


bool RollingMac(uint8_t oui) {
    static char Octet[3];
    sprintf(Octet,"%02X",oui);
    if(Octet[1] == '2') return true;
    if(Octet[1] == '6') return true;
    if(Octet[1] == 'A') return true;
    if(Octet[1] == 'E') return true;
    return false;
}