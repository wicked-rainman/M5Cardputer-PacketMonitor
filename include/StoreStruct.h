#include "Defines.h"
#ifndef STRUCT_H
#define STRUCT_H
typedef struct 
{
    uint8_t MacAddress[6];
    char Ssid[MAX_SSID_LENGTH];
    char Ssid_Assoc[MAX_SSID_LENGTH];
    int32_t Rssi;
    char rolling;
    char mode;
    char fix[40];
} Store;

extern Store storeArray[];
#endif