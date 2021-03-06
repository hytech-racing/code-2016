#include "arduinoMain.h"

int getStatus(unsigned char* message) {
  if(message[0] == 42) {
    return 0; // for normal operation
  }
  else if(message[0] == 25) {
    return 1; // shut down without possibility of recovery
  }
  else if(message[0] == 64) {
     return 2; // stand by
  }
  else {
    return 2;
  }
}

bool AM::getRegenButton(unsigned char* data) {
  if(data[1] == 1) {
    return true;
  }
  else{
    return false;
  }
}
     
