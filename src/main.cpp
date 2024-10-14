#include "main.h"
//-------------------------------------------------------------------
// Main:
// Purpose: 
// 1. Scan the 2.4 band for WiFi networks and store all found
// Mac addresses and SSIDs.
// 2. Force the WiFi card to channel hop, collecting management packets.
// 3. Associate collected management packets with WiFi networks, while
// attempting to associate rolling macs with real macs.
// 4. Associate client mac addresses with WiFi mac addresses/SSIDs.
// 5. On a button press ('l'), associate mac addresses with OUIs.
//
// 8th Oct 2024 - Version 0.9
//
// Note:
// This is really all about trying to squeeze a quart into a pint pot
//-------------------------------------------------------------------


void setup() {
  auto cfg = M5.config();
  M5Cardputer.begin(cfg,true);
  M5.Lcd.setTextSize(1);
  USBSerial.begin(115200);                                                              //Serial port for record output
  Serial1.begin(9600,SERIAL_8N1,1,2);                                                   //Serial port for GPS
  xScreen = xSemaphoreCreateMutex();                                                    //Used by the fuctios in ScreenPrint
  xShmem = xSemaphoreCreateMutex();                                                     //Used by DeviceAdd,Gpsreader,NetworkScan,RolltoFix and keyboardBranch             
  xSemaphoreGive(xScreen);
  xSemaphoreGive(xShmem);
  PacketQueue = xQueueCreate(PACKET_QUEUE_SIZE,12);                                     //Used by WiFiPacketHandler and DeviceAdd
                                                                                        //Initialise the display
  ScreenPrint((char *) "SSID ",5,1,1,TFT_WHITE, TFT_BLACK);                             //Network SSID
  ScreenPrint((char *) "ASSO ",5,3,1,TFT_WHITE, TFT_BLACK);                             //Rolling mac associated SSID
  ScreenPrint((char *) "RECV ",5,5,1,TFT_WHITE, TFT_BLACK);                             //Destination MAC   
  ScreenPrint((char *) "TYPE ",5,5,25,TFT_WHITE,TFT_BLACK);                             //Destination MAC rolling of fixed
  ScreenPrint((char *) "SNDR ",5,7,1,TFT_WHITE, TFT_BLACK);                             //Source MAC
  ScreenPrint((char *) "TYPE ",5,7,25,TFT_WHITE,TFT_BLACK);                             //Source MAC rolling or fixed
  ScreenPrint((char *) "#",1,9,1,TFT_WHITE,TFT_BLACK);                                  //Rec count
  ScreenPrint((char *) "RSSI ",5,9,7,TFT_WHITE,TFT_BLACK);                              //Network RSSI
  ScreenPrint((char *) "MODE ",5,9,16,TFT_WHITE,TFT_BLACK);                             //A=Network added, E=Rolling network edited, D=client
  ScreenPrint((char *) "GPSF ",5,11,1,TFT_WHITE,TFT_BLACK);                             //Current LAT/LON
  ScreenPrint((char *) "TIME ",5,13,1,TFT_WHITE,TFT_BLACK);                             //Current time, TAI or RTC
  DrawCircle(230,50,5,TFT_DARKGREY);                                                    //Battery status
  DrawCircle(230,65,5,TFT_DARKGREY);                                                    //Device added
  DrawCircle(230,80,5,TFT_DARKGREY);                                                    //Network added
  DrawCircle(230,95,5,TFT_DARKGREY);                                                    //Rolling mac association
  DrawCircle(230,110,5,TFT_DARKGREY);                                                   //GPS fix
  DrawCircle(230,125,5,TFT_DARKGREY);                                                   //WiFi channel hop
  SPI.begin(SD_SPI_SCK_PIN, SD_SPI_MISO_PIN, SD_SPI_MOSI_PIN, SD_SPI_CS_PIN);           //Start up SPI for the SD card
  if (!SD.begin(SD_SPI_CS_PIN, SPI, 25000000)) USBSerial.println("SD: Mount FAILED");   //Try and mount the SD card

  wifi_init_config_t cfg1 = WIFI_INIT_CONFIG_DEFAULT();                                 //Configure wifi.
  esp_wifi_init(&cfg1);
  //esp_wifi_set_country(&wifi_country); 
  esp_wifi_set_storage(WIFI_STORAGE_RAM);
  esp_wifi_set_mode(WIFI_MODE_NULL);
  esp_wifi_start();
  esp_wifi_set_promiscuous(true);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
                                                                                        //Create required tasks
  xTaskCreatePinnedToCore(BatteryStatus,"BS",5000,NULL,0,&tasks[TaskCount++],1);        //Start up the battery monitor
  xTaskCreatePinnedToCore(GpsReader,"GR",5000,NULL,0,&tasks[TaskCount++],1);            //Start up the GPS reader
  while(!GpsLock) vTaskDelay(100);                                                      //Wait for an initial gps fix  
  xTaskCreatePinnedToCore(NetworkScan,"NS",4096,NULL,0,&tasks[TaskCount++],0);          //Start up the network scanner
  vTaskDelay(NETWORK_SCAN_INTERVAL+5000);                                               //Give time for network discovery
  xTaskCreatePinnedToCore(RollToFix,"RF",4096,NULL,0,&tasks[TaskCount++],1);            //Start the rolling mac editor
  vTaskDelay(1000);                                                                     //Allow initial rolling mac fix time for networks
  esp_wifi_set_promiscuous_rx_cb(&WiFiPacketHandler);                                   //Start up management packet grabbing
  xTaskCreatePinnedToCore(ChannelHop,"CH",4096,NULL,0,&tasks[TaskCount++],0);           //Start up channel hopping
  xTaskCreatePinnedToCore(DeviceAdd,"DA",4096,NULL,0,&tasks[TaskCount++],0);            //Start collecting mac addresses
  xTaskCreatePinnedToCore(KeyboardBranch,"KB",4096,NULL,0,&tasks[TaskCount],1);         //Start keyboard interaction
}

void loop() { vTaskDelay(60);}
