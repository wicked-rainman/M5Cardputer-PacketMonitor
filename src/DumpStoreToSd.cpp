#include "DumpStoreToSd.h"
void CharReplace(char *, char, char);
int CachePos=0;
//----------------------------------------------------------------------
// Function: DumpNetworks
// Args: none
//
// External int StoreLastUsed indicates the last used record in external array storeArray.
// Contents of storeArray is read, formatted and output to USBSerial.
// This function is either called because the storeArray is full,
// or just prior to the whole file contents being dumped.
//----------------------------------------------------------------------

void DumpNetworks() {
    extern int StoreLastUsed;
    static int k;
    static char msg[160];
    File file = SD.open("/Networks.txt",FILE_APPEND);
    if(file) {
        for(k=0;k<StoreLastUsed;k++) {
            snprintf(msg,160,"%03d,%c,%02X%02X%02X%02X%02X%02X,%c,%d,%s,%s,%s\n",
                k,storeArray[k].mode,
                storeArray[k].MacAddress[0],storeArray[k].MacAddress[1],storeArray[k].MacAddress[2],
                storeArray[k].MacAddress[3],storeArray[k].MacAddress[4],storeArray[k].MacAddress[5],
                storeArray[k].rolling, storeArray[k].Rssi,storeArray[k].Ssid, storeArray[k].Ssid_Assoc, storeArray[k].fix);
            file.printf("%s",msg);
        }
        file.close();
    }
    else {
        if(GENERATE_SERIAL_OUTPUT) USBSerial.println("Couldn't open network dump output file");
    }
}
//----------------------------------------------------------------------
// Function: DumpDevices
// Args: None
//
// Output the contents of "/Devices.txt" to USBSerial.
// External integer DeviceCount indicates how many records in
// the external array DeviceTable need to be written.
// This function is either called because the DeviceTable is full,
// or just prior to the whole file contents being dumped.
//----------------------------------------------------------------------

void DumpDevices() {
    extern int DeviceCount;
    extern char DeviceTable[][DEVICE_TABLE_RECSIZE];
    static int k;
    static char msg[80];
    File file = SD.open("/Devices.txt",FILE_APPEND);
    if(file) {
        for(k=0;k<DeviceCount;k++) file.printf("%s",DeviceTable[k]);
        file.close();
    }
    else USBSerial.println("Couldn't open device dump output file");
}
//----------------------------------------------------------------------
// Function: DumpFile
// Args: character pointer to the file that should be dumped.
//
// Try and open the specified file, then read until EOF, writing to 
// USBSerial. If the records being read are type A or D and are fixed
// Mac addresses, then do a (slow) OUI lookup.
// This function only ever gets called when the dump ('d') button
// is pressed on the keyboard.
//----------------------------------------------------------------------

void DumpFile(char *fname, bool OuiLookup) {
    File file = SD.open(fname,FILE_READ);
    char InputLine[200];
    char MacStrSender[7];
    char MacStrReceiver[7];
    char OuiStr[150];
    int k=0;
    int ReccordCount=1;
    if(file) {
        while (file.available()) {
            k=file.readBytesUntil('\n',InputLine,200);
            InputLine[k]=0x0;
            if(OuiLookup) {
                memset(OuiStr,0,150);
                if(InputLine[19]=='F') {
                    memcpy(MacStrSender,InputLine+6,6);
                    snprintf(OuiStr,150,",%s",LookupOui(MacStrSender));
                }
                else snprintf(OuiStr,150,",");
                if(InputLine[4]=='D') {
                    if (InputLine[34]=='F') {
                        memcpy(MacStrReceiver,InputLine+21,6);
                        strcat(OuiStr,",");
                        strcat(OuiStr,LookupOui(MacStrReceiver));
                    }
                    else strcat(OuiStr,",");
                }
                USBSerial.printf("%s%s\n",InputLine,OuiStr);
            }
            else USBSerial.printf("%s\n",InputLine);
            M5Cardputer.Display.setCursor(60,25);
            M5Cardputer.Display.fillRect(60,24,110,20,TFT_BLACK);
            M5Cardputer.Display.printf("%d",ReccordCount++);
        }
        file.close();
    }
    else USBSerial.printf("//<<<<<<<<<<<<<<<<<<Failed to open %s >>>>>>>>>>>>>>>>\n", fname);
}
//----------------------------------------------------------------------
// Function LookupOui
// Args: String of first 3 bytes in Hex
// This is all about being as fast as possible as the OUI lookup file
// is big. Order of processing is:
// 1.   If the query mac is the same as the last one looked up, just 
//      return the previous result
// 2.   If the mac has to be looked up in the file, cache the results
//      and check the cache before the file
// 3.   Mac is different, not in the cache, so look it up in the file
//      then cache that result (Round robin fixed cache size)
//----------------------------------------------------------------------

char *LookupOui(char *TargetMac) {
    char Line[200];
    static char ReturnStr[MAX_OUI_VENDOR_LEN+5];
    static char Cache[OUI_CACHE_SIZE][MAX_OUI_VENDOR_LEN];
    extern int CachePos;
    static char PreviousMac[7];
    static File file;
    static File eq00;
    static File gt00;
    char debug[6];
    int k;
    //---------------------------------------------------------------------
    //The OUI table has around 32k entries in it.
    //One third of these entries are for mac addresses tha start 00h.
    //Open one file pointer to the file that contains 00h records and
    //another file pointer for mac addresses greater than 00h.
    //This helps speed up the lookups
    //---------------------------------------------------------------------
    if(!eq00) {
        eq00=SD.open("/00Oui.txt");
        if(!eq00) {
            strncpy(ReturnStr,"Error",5);
            ReturnStr[5]=0x0;
            return ReturnStr;
        }        
    }

    if(!gt00) {
        gt00=SD.open("/GT00Oui.txt");
        if(!gt00) {
            strncpy(ReturnStr,"Error",5);
            ReturnStr[5]=0x0;
            return ReturnStr;
        }        
    }


    // Is the left half of the mac being looked up the same as previous lookup ?
    if (!memcmp(TargetMac,PreviousMac,6)) return ReturnStr;

    memcpy(PreviousMac,TargetMac,6);
    //Is the left half of the mac in cache ?
    for(k=0;k<OUI_CACHE_SIZE;k++) {
        if(!memcmp(Cache[k],TargetMac,6)) {
            memcpy(ReturnStr,Cache[k]+7,MAX_OUI_VENDOR_LEN-7);
            ReturnStr[MAX_OUI_VENDOR_LEN-7]=0x0;
            //snprintf(debug,6," #%03d",k);
            //strcat(ReturnStr,debug);
            return ReturnStr;
        }    
    }

    //Not in cache, so look in oui file (Doh!)
    //Choose which lookup file to use
    if(!memcmp(TargetMac,"00",2)) file = eq00;
    else file=gt00;
    file.seek(0);
    while(file.available()) {
        k=file.readBytesUntil('\n',Line,200);
        if(!memcmp(Line,TargetMac,6)) {
            if(k>MAX_OUI_VENDOR_LEN) {
                Line[MAX_OUI_VENDOR_LEN]=0x0;
                CharReplace(Line,',',' ');
                k=MAX_OUI_VENDOR_LEN;
            }
            else Line[k]=0x0;
            memset(Cache[CachePos],0,MAX_OUI_VENDOR_LEN);
            memcpy(Cache[CachePos],Line,k);
            memcpy(ReturnStr,Cache[CachePos++]+7,k-7);
            ReturnStr[k-7]=0x0;
            if(CachePos==OUI_CACHE_SIZE) CachePos=0;
            return ReturnStr;
        }
    }
    strncpy(ReturnStr,"NotFound\0",9);
    return ReturnStr;
}