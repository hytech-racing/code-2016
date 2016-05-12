#include "arduinoMain.h"



bool AM::getRegenButton(unsigned char* data) {
  if(data[1] == 1) {
    return true;
  }
  else{
    return false;
  }
}
     
