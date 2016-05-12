#include "ar.h"

int AR::getError(unsigned char* data) {
        int errorCode;
        int errorCodeSecond;
        
        if(data[0] == 1) {
          errorCode = 1; // battery overheat
        }
        
        return errorCode;
        
}

int AR::getResetButtons(unsigned char* data){
  return data[3];
}

int AR::getVoltage(unsigned char* data) {
  return data[2];
}
