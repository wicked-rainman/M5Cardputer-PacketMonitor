#include "ScreenPrint.h"
extern SemaphoreHandle_t xScreen;
//Screen size is 240*135
//Font size 1:
//17 lines,39 characters, so char height=8 and width=6
//
void ScreenPrint(char *msg, uint8_t len, uint8_t Row, uint8_t Col, uint16_t FontColour, uint16_t BackgroundColour) {
    static int16_t ColSize=6;
    static int16_t RowSize=8;
    static char TheText[50];
    if(xSemaphoreTake(xScreen,500)) {
        M5Cardputer.Display.setCursor(Col*ColSize,Row*RowSize);
        M5Cardputer.Display.setTextSize(1);
        M5Cardputer.Display.fillRect(Col*ColSize,Row*RowSize,len*ColSize,RowSize,TFT_BLACK);
        M5Cardputer.Display.setTextColor(FontColour,BackgroundColour);
        strncpy(TheText,msg,len);
        TheText[len]=0x0;
        M5Cardputer.Display.print(TheText);
        xSemaphoreGive(xScreen);
    }
}


void DrawCircle(int32_t col, int32_t row, int32_t radius,int tftColour) {
    extern SemaphoreHandle_t xScreen;
    if(xSemaphoreTake(xScreen,500)) {
        M5Cardputer.Display.fillCircle(col,row,radius,tftColour);
        xSemaphoreGive(xScreen);
    }
}

void DrawRect(int32_t col, int32_t row, int32_t width, int32_t height, int tftcolour) {
    extern SemaphoreHandle_t xScreen;
    if(xSemaphoreTake(xScreen,500)) {
        M5Cardputer.Display.fillRect(col,row,width,height,tftcolour);
        xSemaphoreGive(xScreen);
    }
}