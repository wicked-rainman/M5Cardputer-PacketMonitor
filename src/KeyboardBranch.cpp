#include "KeyboardBranch.h"
//-----------------------------------------------------------------
// Function KeyboardBranch
// Purpose: to allow dumping of found WiFi networks and macs to serial out.
// Provisions for looking up MAC OUI values.
//-----------------------------------------------------------------
void KeyboardBranch(void *abc) {
    extern SemaphoreHandle_t xShmem;
    extern TaskHandle_t tasks[];
    extern int TaskCount;
    static int k;
    char Reason;
    while(true) {
        M5Cardputer.update();
        if (M5Cardputer.Keyboard.isChange()) {
            if (M5Cardputer.Keyboard.isKeyPressed('d')) Reason='d';
            else if (M5Cardputer.Keyboard.isKeyPressed('l')) Reason='l';
            else if (M5Cardputer.Keyboard.isKeyPressed('w')) Reason='w';
            else {};
            if(Reason=='d' || Reason=='l') {
                xSemaphoreTake(xShmem,portMAX_DELAY);
                for(k=0;k<TaskCount;k++) vTaskDelete(tasks[k]); //K=1 because this is the first task
                M5Cardputer.Display.fillScreen(TFT_BLACK);
                M5Cardputer.Display.setTextColor(TFT_GREEN);
                M5Cardputer.Display.setTextSize(2);
                M5Cardputer.Display.setCursor(5,5);
                M5Cardputer.Display.println("WAIT");
                DumpNetworks(); //Write current network cache
                DumpDevices();  //Write current device cache
                USBSerial.println("\n//-------<<<<<<<<<<<<<<<<<<<<< DUMP start >>>>>>>>>>>>>>>>>>>>------//");
                if(Reason=='d') {
                    DumpFile((char *) "/Networks.txt",false); //Dump whole network file
                    DumpFile((char *) "/Devices.txt",false);  //Dump whole device file
                }
                else {//Lookup OUI dump selected (l)
                    M5Cardputer.Display.setCursor(5,25);
                    M5Cardputer.Display.println("Rec: ");
                    DumpFile((char *) "/Networks.txt",true); //Dump whole network file and resolve OUIs
                    DumpFile((char *) "/Devices.txt",true);  //Dump whole device file amd resolve OUIs
                }
                USBSerial.println("\n//<<<<<<<<<<<<<<<<<<DUMP end >>>>>>>>>>>>>>>>>>>>//");
                M5Cardputer.Display.fillScreen(TFT_BLACK);
                M5Cardputer.Display.setCursor(45,5);
                M5Cardputer.Display.println("Done");
                while(1);
            }
            else if (Reason=='w') {
                xSemaphoreTake(xShmem,portMAX_DELAY);
                for(k=0;k<TaskCount;k++) vTaskDelete(tasks[k]); //K=1 because this is the first task
                M5Cardputer.Display.fillScreen(TFT_BLACK);
                M5Cardputer.Display.setTextColor(TFT_GREEN);
                M5Cardputer.Display.setTextSize(3);
                M5Cardputer.Display.setCursor(10,5);
                M5Cardputer.Display.println("WAIT");
                SD.remove((char *) "/Networks.txt");
                SD.remove((char *) "/Devices.txt");
                M5Cardputer.Display.fillScreen(TFT_BLACK);
                M5Cardputer.Display.setCursor(10,5);
                M5Cardputer.Display.println("Done");
                USBSerial.println("\n//<<<<<<<<<<<<<<<<<<Erase Completed >>>>>>>>>>>>>>>>>>>>//");
                while(1);
            }
            else {
                USBSerial.println("Other key pushed");
            }
        }
        vTaskDelay(KEYBOARD_BRANCH_INTERVAL);
    }
}