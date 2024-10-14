#include "RollingMac.h"
bool RollingMac(uint8_t oui) {
    static char Octet[3];
    sprintf(Octet,"%02X",oui);
    if(Octet[1] == '2') return true;
    if(Octet[1] == '6') return true;
    if(Octet[1] == 'A') return true;
    if(Octet[1] == 'E') return true;
    return false;
}