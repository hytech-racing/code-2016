#include "ar.h"

int AR::getError(unsigned char* data) {
        int errorCodeFirst; // this can have a value of 0,1,2,4,5,6,8,9,10
        int errorCodeSecond;
        
        if(data[0] == 1) {
          errorCode 4;
        }
        
        if(data[1] == 1) {
          errorCode = 3;
        }
        
        if(data[0] == 2) {
          errorCode = 2;
        }

        if(data[1] == 2) {
          errorCode = 1;
        }
        
        
}
