#include "BatteryStatus.h"
//----------------------------------------------------------------------
//Function BatteryStatus
//Args: None used
//
//RTOS task to draw a visual representation of the battery state.
//Updates (if battery state has changed) every 60 seconds.
//----------------------------------------------------------------------
void BatteryStatus(void *something) {
    static int32_t BatteryPercentage=1; 
    static int32_t PreviousCharge=0;

    while(true) {
        BatteryPercentage=M5Cardputer.Power.getBatteryLevel();
        if(BatteryPercentage!=PreviousCharge) {
            if(BatteryPercentage>75) DrawCircle(230,50,5,TFT_GREEN);
            else if(BatteryPercentage>50) DrawCircle(230,50,5,TFT_YELLOW);
            else if(BatteryPercentage>25) DrawCircle(230,50,5,TFT_ORANGE);
            else   DrawCircle(230,50,5,TFT_RED);
        }
        PreviousCharge=BatteryPercentage;
        vTaskDelay(60000);
    }
}