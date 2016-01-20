#include "EVDC.h"

int EVDC::getError(unsigned char* data) {
  return int(data[0]);
}

int EVDC::getButtons(unsigned char* data) {
  return int(data[1]); // lsb is shutdown button, 2ndlsb is progress button
}

int EVDC::getBrakes(unsigned char* data) {
  return int(data[2]);
}
