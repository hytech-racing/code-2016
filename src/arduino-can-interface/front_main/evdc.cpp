#include "EVDC.h"

int EVDC::getError(unsigned char* data) {
  return int(data[0]);
}

int EVDC::getButtons(unsigned char* data) {
  return int(data[1]); // lsb is shutdown button, 2ndlsb is progress button
}

int EVDC::getBrakes(unsigned char* data) { // from 0 to 100
  return int(data[2]);
}

void EVDC::goForLaunch(MCP_CAN& CanYouDoTheCANCAN) {
  unsigned char EVDCGoMsgBuf[8] = {42,42,42,42,42,42,42,42};
  CanYouDoTheCANCAN.sendMsgBuf(transmitMessage, 0, 8, EVDCGoMsgBuf);

}
  
void EVDC::shutThemAllDown(MCP_CAN& CanYouDoTheCANCAN) {
  
  unsigned char EVDCStopMsgBuf[8] = {25,25,25,25,25,25,25,25};
  CanYouDoTheCANCAN.sendMsgBuf(transmitMessage, 0, 8, EVDCStopMsgBuf);
}

void EVDC::calmTheFreakDown(MCP_CAN& CanYouDoTheCANCAN) {
    unsigned char EVDCStandbyMsgBuf[8] = {64,64,64,64,64,64,64,64};
    CanYouDoTheCANCAN.sendMsgBuf(transmitMessage, 0, 8, EVDCStandbyMsgBuf);
}


  
  
