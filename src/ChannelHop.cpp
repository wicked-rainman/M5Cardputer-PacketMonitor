#include "ChannelHop.h"
//----------------------------------------------------------------------
// Function: ChannelHop
// Args: None used
//
// An RTOS task to set the WiFi card channel number.
// Channel is defined by ChannelSequence, swapping every 0.3 seconds
//----------------------------------------------------------------------

void ChannelHop(void *abc) {
  static uint8_t channelSequence[CHANNEL_COUNT]={1,6,11,3,4,1,6,11,8,9,1,6,11,2,7,1,6,11,5,10};
  static uint8_t channel=0;
  static bool OnOff;
  while(true) {
    esp_wifi_set_channel(channelSequence[channel++], WIFI_SECOND_CHAN_NONE);
   if(channel == CHANNEL_COUNT) channel = 0;
    OnOff ?   DrawCircle(230,125,5,TFT_PURPLE) : DrawCircle(230,125,5,TFT_CYAN);
    OnOff = !OnOff;
    vTaskDelay(300);
  }
}