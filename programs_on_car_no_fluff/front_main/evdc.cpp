#include "evdc.h"

int EVDC::getError(unsigned char* data) { // first bit is high if there is a pedal error. second bit is high if the EVDC is not launched
  return int(data[2]); // will be 1 if pedal error exists
}

int EVDC::getButtons(unsigned char* data) {
  return int(data[3]); // lsb is shutdown button, 2ndlsb is progress button
}

int EVDC::getBrakes(unsigned char* data) { // from 0 to 100
  return int(data[1]);
}

int EVDC::getAccelerator(unsigned char* data) {
  return int(data[3]); 
}

int EVDC::getLaunchedState(unsigned char* data) {
  return data[0];
}

void EVDC::goForLaunch(MCP_CAN& CanYouDoTheCANCAN) {
  unsigned char EVDCGoMsgBuf[8] = {42,42,42,42,42,42,42,42};
  CanYouDoTheCANCAN.sendMsgBuf(transmitMessage, 0, 8, EVDCGoMsgBuf);

}
  
void EVDC::shutThemAllDown(MCP_CAN& CanYouDoTheCANCAN) {
  
  unsigned char EVDCStopMsgBuf[8] = {25,25,25,25,25,25,25,25};
  CanYouDoTheCANCAN.sendMsgBuf(transmitMessage, 0, 8, EVDCStopMsgBuf);
}

void EVDC::calmTheFreakDown(MCP_CAN& CanYouDoTheCANCAN, int code = 64) {
    unsigned char EVDCStandbyMsgBuf[8] = {64,64,64,64,64,64,64,64};
    EVDCStandbyMsgBuf[0] = code;
    CanYouDoTheCANCAN.sendMsgBuf(transmitMessage, 0, 8, EVDCStandbyMsgBuf);
}


  
  
