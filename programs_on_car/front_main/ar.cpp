#include "ar.h"

int AR::getError(unsigned char* data) {
        int errorCode;
        int errorCodeFirst; // this can have a value of 0,1,2,4,5,6,8,9,10
        int errorCodeSecond;
        
        if(data[0] == 1) {
          errorCode = 4; // battery overheat
        }
        
        if(data[1] == 1) {
          errorCode = 3; // battery bad voltage
        }
        
        if(data[0] == 2) {
          errorCode = 2; // battery getting warm
        }

        if(data[1] == 2) {
          errorCode = 1; // battery low voltage
        }
        
        return errorCode;
        
        
}

int AR::getResetButtons(unsigned char* data){
  return data[3];
}

int AR::getVoltage(unsigned char* data) {
  return data[2];
}
