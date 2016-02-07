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

void EVDC::goForLaunch(MCP_CAN& CanYouDoTheCANCAN) {
  unsigned char EVDCGoMsgBuf[8] = {42,42,42,42,42,42,42,42};
  CanYouDoTheCANCAN.sendMsgBuf(0x1DC, 0, 8, EVDCGoMsgBuf);

}
  
void EVDC::shutThemAllDown(MCP_CAN& CanYouDoTheCANCAN) {
  
  unsigned char EVDCStopMsgBuf[8] = {0,1,4,9,16,25,36,49};
  CanYouDoTheCANCAN.sendMsgBuf(0x1DC, 0, 8, EVDCStopMsgBuf);
}
  
  
