#include "GpsReader.h"
//--------------------------------------------------------------
// Function GpsReader
// Purpose: Extracts time and location from $GNRMC messages
// that have been read from the GPS attached to the serial port.
// Sets the RTC so that if the GPS signal is lost, records still
// have a time stamp.
//--------------------------------------------------------------
void GpsReader(void *whereami) {
    extern SemaphoreHandle_t xShmem;
    extern ESP32Time rtc;
    extern char GpsFix[];
    extern bool GpsLock;
    static bool RtcLock;
    char ch=0x0;
    char hours[3];
    char mins[3];
    char secs[3];
    char Latitude[14];
    char Longitude[14];
    uint8_t SentenceSize=0;
    static char GpsSentence[MAX_GPS_SENTENCE_SIZE];
    while(true) {
        DrawCircle(230,110,5,TFT_BLUE);
        while(ch !='$') {
            if(Serial1.available()) ch = Serial1.read();
        }
        SentenceSize=1;
        GpsSentence[0]=ch;
        while ((ch != '\n') && (SentenceSize < MAX_GPS_SENTENCE_SIZE)) {
            if (Serial1.available()) {
                ch = Serial1.read();
                GpsSentence[SentenceSize++] = ch;
            }
        }
        GpsSentence[strlen(GpsSentence)-2] = '\0';
        if(!memcmp("$GNRMC",GpsSentence,6) && (GpsSentence[18]==GOOD_GPS_FIX)){
            if(SentenceSize>71 && SentenceSize<75) { 
                DrawCircle(230,110,5,TFT_GREEN);
                memcpy(Latitude,GpsSentence+20,10);
                Latitude[10]=GpsSentence[31];
                Latitude[11] = '\0';
                memcpy(Longitude,GpsSentence+33,11);
                Longitude[11]=GpsSentence[45];
                Longitude[12]='\0';
                memcpy(hours,GpsSentence+7,2);
                memcpy(mins,GpsSentence+9,2);
                memcpy(secs,GpsSentence+11,2);
                if(xSemaphoreTake(xShmem,1000)) {
                    if(!RtcLock) {
                        RtcLock=true;
                        rtc.setTime(atoi(secs),atoi(mins),atoi(hours), 17, 1, 2024); 
                    }
                    GpsLock=true;
;                   snprintf(GpsFix,40,"%s:%s:%s %s %s",hours,mins,secs,Latitude,Longitude);
                    xSemaphoreGive(xShmem);
                }
            }
            else {
                USBSerial.printf("#GPS ERROR: %d \"%s\"\n",SentenceSize,GpsSentence);
                GpsLock=false;
                RtcLock=false;
                DrawCircle(230,110,5,TFT_ORANGE);
            }
            vTaskDelay(GPS_READ_INTERVAL);
        }
    }
}