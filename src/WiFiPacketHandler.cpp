#include "WiFiPacketHandler.h"
//-----------------------------------------------------------
// Function: WiFiPacketHandler
// Purpose: Places previously unseen mac address pairs on 
// an fifo queue for processing by DeviceAdd
//-----------------------------------------------------------
void WiFiPacketHandler(void* buff, wifi_promiscuous_pkt_type_t type) {
    static int k;
    static int LastUsed=0;
    static bool found=false;
    static uint8_t DoubleMac[12];
    static char msg[80];
    static uint8_t bcast[6]={255,255,255,255,255,255};
    static uint8_t BeaconStore[BEACON_CACHE_SIZE][12];
    extern uint8_t NewMacs[];
    extern QueueHandle_t PacketQueue;

    if (type != WIFI_PKT_MGMT) return;
    const wifi_promiscuous_pkt_t *ppkt = (wifi_promiscuous_pkt_t *)buff;
    const wifi_ieee80211_packet_t *ipkt = (wifi_ieee80211_packet_t *)ppkt->payload;
    const wifi_ieee80211_mac_hdr_t *hdr = &ipkt->hdr;

    if(memcmp(hdr->addr1,bcast,6)) {
        memcpy(DoubleMac,hdr->addr1,6);
        memcpy(DoubleMac+6,hdr->addr2,6);
        if(LastUsed==BEACON_CACHE_SIZE) LastUsed=0;
        for(k=0;k<BEACON_CACHE_SIZE;k++) {
            if(!memcmp(DoubleMac,BeaconStore[k],12)) {
                found=true;
                break;
            }
        }
        if(!found) {
            xQueueSend(PacketQueue, (void*) DoubleMac, (TickType_t)0);
            memcpy(BeaconStore[LastUsed],hdr->addr1,6);
            memcpy(BeaconStore[LastUsed++]+6,hdr->addr2,6);
        }
        found=false;
    }
}