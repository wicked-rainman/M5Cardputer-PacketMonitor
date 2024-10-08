#include <string.h>
//----------------------------------------------------------------------
// Function CharReplace
// Args: Target string, char to find, char to replace
// Note: The output is generated with a space between values.
// This is used to remove any space characters in vield values.
//----------------------------------------------------------------------

void CharReplace(char *target, char FromChar, char ToChar) {
    int k;
    int len;
    len=strlen(target);
    for(k=0;k<len;k++) {
        if(target[k] == FromChar) target[k]=ToChar;
    }
}