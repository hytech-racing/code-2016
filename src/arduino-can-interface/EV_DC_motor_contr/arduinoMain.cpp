#include "arduinoMain.h"

int getStatus(unsigned char* message) {
  if(message[0] == 42) {
    return 0; // for normal operation
  }
  else if(message[0] == 25) {
    return 1; // shut down without possibility of recovery
  }
  else if(message[0] == 55) {
     return 2; // stand by
  }
  else {
    return 2;
  }
}
     
