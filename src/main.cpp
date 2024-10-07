#include "main.h"
void setup() {
  auto cfg = M5.config();
  M5Cardputer.begin(cfg,true);
  M5.Lcd.setTextSize(1);
  USBSerial.begin(115200);
  Serial1.begin(9600,SERIAL_8N1,1,2);
  xScreen = xSemaphoreCreateMutex();
  xShmem = xSemaphoreCreateMutex();
  xSemaphoreGive(xScreen);
  xSemaphoreGive(xShmem);
  PacketQueue = xQueueCreate(20,12); 

  //Initialise the display
  ScreenPrint("SSID ",5,1,1,TFT_WHITE,TFT_BLACK);
  ScreenPrint("ASSO ",5,3,1,TFT_WHITE,TFT_BLACK);
  ScreenPrint("RECV ",5,5,1,TFT_WHITE,TFT_BLACK);
  ScreenPrint("TYPE ",5,5,25,TFT_WHITE,TFT_BLACK);
  ScreenPrint("SNDR ",5,7,1,TFT_WHITE,TFT_BLACK);
  ScreenPrint("TYPE ",5,7,25,TFT_WHITE,TFT_BLACK);
  ScreenPrint("TYPE ",5,7,25,TFT_WHITE,TFT_BLACK);
  ScreenPrint("#",1,9,1,TFT_WHITE,TFT_BLACK);
  ScreenPrint("RSSI ",5,9,7,TFT_WHITE,TFT_BLACK);
  ScreenPrint("MODE ",5,9,16,TFT_WHITE,TFT_BLACK);
  ScreenPrint("GPSF ",5,11,1,TFT_WHITE,TFT_BLACK);
  ScreenPrint("TIME ",5,13,1,TFT_WHITE,TFT_BLACK);
  DrawCircle(230,50,5,TFT_DARKGREY);
  DrawCircle(230,65,5,TFT_DARKGREY);
  DrawCircle(230,80,5,TFT_DARKGREY);
  DrawCircle(230,95,5,TFT_DARKGREY);
  DrawCircle(230,110,5,TFT_DARKGREY);
  DrawCircle(230,125,5,TFT_DARKGREY);

//Start storage card
 SPI.begin(SD_SPI_SCK_PIN, SD_SPI_MISO_PIN, SD_SPI_MOSI_PIN, SD_SPI_CS_PIN);
 if (!SD.begin(SD_SPI_CS_PIN, SPI, 25000000)) USBSerial.println("SD: Mount FAILED");

//Set up WiFi
  nvs_flash_init();
  tcpip_adapter_init();
  wifi_init_config_t cfg1 = WIFI_INIT_CONFIG_DEFAULT();
  esp_wifi_init(&cfg1);
  esp_wifi_set_country(&wifi_country); 
  esp_wifi_set_storage(WIFI_STORAGE_RAM);
  esp_wifi_set_mode(WIFI_MODE_NULL);
  esp_wifi_start();
  esp_wifi_set_promiscuous(true);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  xTaskCreatePinnedToCore(GpsReader,"GR",5000,NULL,0,&tasks[TaskCount++],1);        //Start up the GPS reader
  while(!GpsLock) vTaskDelay(100);                                                  //Wait for a gps fix  
  xTaskCreatePinnedToCore(BatteryStatus,"BS",5000,NULL,0,&tasks[TaskCount++],1);    //Start up the battery monitor
  xTaskCreatePinnedToCore(NetworkScan,"NS",4096,NULL,0,&tasks[TaskCount++],0);      //Start up the network scanner
  vTaskDelay(10000);                                                                //Give time for network discovery
  xTaskCreatePinnedToCore(RollToFix,"RF",4096,NULL,0,&tasks[TaskCount++],1);        //Start the rolling mac editor
  vTaskDelay(1000);                                                                 //Allow rolling mac fix time for networks
  esp_wifi_set_promiscuous_rx_cb(&WiFiPacketHandler);                               //Start up management packet grabbing
  xTaskCreatePinnedToCore(ChannelHop,"CH",4096,NULL,0,&tasks[TaskCount++],0);       //Start up channel hopping
  xTaskCreatePinnedToCore(DeviceAdd,"DA",4096,NULL,0,&tasks[TaskCount++],0);        //Start collecting mac addresses
  xTaskCreatePinnedToCore(KeyboardBranch,"KB",4096,NULL,0,&tasks[TaskCount],1);     //Start keyboard interaction
}

void loop() { vTaskDelay(60);}
