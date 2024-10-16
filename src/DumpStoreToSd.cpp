#include "DumpStoreToSd.h"
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
// Args: character pointer to the file that should be dumped, and a
// boolean indicating if OUI lookups should be performed.
//
// Try and open the specified file, then read until EOF, writing to 
// USBSerial. If the records being read contain a fixed
// Mac address and OuiLookup is true, then do an OUI lookup. 
// If the record is for a device (D in column 4) then there might be two 
// fixed macs to look up.
// This function gets called when the 'd' or 'l' button
// is pressed on the keyboard (dump, or dump and lookup).
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
            if(OuiLookup) {                                                             //If OUI lookups specified
                memset(OuiStr,0,150);
                if(InputLine[19]=='F') {                                                //If first mac address is fixed
                    memcpy(MacStrSender,InputLine+6,6);
                    snprintf(OuiStr,150,",%s",LookupOui(MacStrSender));                 //Look up the OUI
                }
                else snprintf(OuiStr,150,",");                                          //Not fixed mac
                if(InputLine[4]=='D') {                                                 //Devices record, so second mac to check
                    if (InputLine[34]=='F') {                                           //Second mac is fixed
                        memcpy(MacStrReceiver,InputLine+21,6);
                        strcat(OuiStr,",");
                        strcat(OuiStr,LookupOui(MacStrReceiver));
                    }
                    else strcat(OuiStr,",");                                            //Second mac is floating
                }
                USBSerial.printf("%s%s\n",InputLine,OuiStr);                            //Output the record
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