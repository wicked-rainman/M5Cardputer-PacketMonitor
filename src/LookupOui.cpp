//----------------------------------------------------------------------
// Function LookupOui
// Args: String of first 3 bytes in Hex
// This is all about being as fast as possible as the OUI lookup file
// is big. Order of processing is:
// 1.   If the query mac is the same as the last one looked up, just 
//      return the previous result (Good for sequence exchanges)
// 2.   If the mac has to be looked up in the file, cache the results
//      and check the cache before reverting to the file
// 3.   Mac is different, not in the cache, so look it up in the file
//      then cache that result (Round robin fixed cache size)
//
// Note: This could be made quicker by looking up 3 bytes as a binary
// but the distributed OUI file carrys ascii hex values. I suspect
// most of the time goes in reading the SD card, so the speed benefits
// in changing this might be negligible. To make it faster, either the
// OUI file needs to be devided up more, or the whole file stored in
// flash......
//----------------------------------------------------------------------
#include "LookupOui.h"
char *LookupOui(char *TargetMac) {
    char Line[200];
    static char ReturnStr[MAX_OUI_VENDOR_LEN+5];
    static char Cache[OUI_CACHE_SIZE][MAX_OUI_VENDOR_LEN];
    static int CachePos=0;
    static char PreviousMac[7];
    static File file;
    static File eq00;
    static File gt00;
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
        if(!eq00) return (char *) "Error";        
    }

    if(!gt00) {
        gt00=SD.open("/GT00Oui.txt");
        if(!gt00) return (char *) "Error";       
    }
    if (!memcmp(TargetMac,PreviousMac,6)) return ReturnStr;                             //Vendor same as previous lookup
    memcpy(PreviousMac,TargetMac,6);                                                    //Wasn't the same, so store it.

                                                                                        //Is the left half of the mac in cache ?
    for(k=0;k<OUI_CACHE_SIZE;k++) {
        if(!memcmp(Cache[k],TargetMac,6)) {                                             //Vendor found in cache
            memcpy(ReturnStr,Cache[k]+7,MAX_OUI_VENDOR_LEN-7);
            ReturnStr[MAX_OUI_VENDOR_LEN-7]=0x0;
            return ReturnStr;                                                           //Return vendor string
        }    
    }
                                                                                        //Not in cache, so look in oui file (Doh!)
    (!memcmp(TargetMac,"00",2)) ? (file = eq00) : (file=gt00);                          //Pick which file to use for lookup
    file.seek(0);                                                                       //Go to the start of selected file
    while(file.available()) {                                                           //Start reading OUI 
        k=file.readBytesUntil('\n',Line,200);
        if(!memcmp(Line,TargetMac,6)) {  
            CharReplace(Line,',',' ');                                                  //Remove any commas out of vendor string                                               //If mac matches
            if(k>MAX_OUI_VENDOR_LEN) {                                                  //Cut off vendor length if it's too long
                Line[MAX_OUI_VENDOR_LEN]=0x0;
                k=MAX_OUI_VENDOR_LEN;
            }
            else Line[k]=0x0;                                                           //Line wasn't too long
            memset(Cache[CachePos],0,MAX_OUI_VENDOR_LEN);                               //Empty a slot in the cache
            memcpy(Cache[CachePos],Line,k);                                             //Add result in for next time
            memcpy(ReturnStr,Cache[CachePos]+7,k-7);                                    //Prepair vendor output
            ReturnStr[k-7]=0x0;
            (CachePos==OUI_CACHE_SIZE) ? CachePos=0: CachePos++;                        //Move cache pointer on or RR.
            return ReturnStr;                                                           //Return found vendor string
        }
    }
    strncpy(ReturnStr,"NotFound\0",9);                                                  //Vendor not found!
    return ReturnStr;
}