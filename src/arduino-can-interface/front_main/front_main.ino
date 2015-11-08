#include <SPI.h>
#include "mcp_can.h"
#include "mc.h"
#include "evdc.h"
#include "bms.h"
#include "ar.h"
#include "pi.h"
#include "startupShutdown.h"

#define BMS_TIMED_OUT 1
#define EVDC_TIMED_OUT 2
#define MC_TIMED_OUT 3
#define REAR_TIMED_OUT 4

long BMS_timeout;
long BMS_timeout_limit = 2000; // 2000 ms
long ar_timeout;
long ar_timeout_limit = 2000;
long MC_timeout;
long MC_timeout_limit = 500;
long EVDC_timeout;
long EVDC_timmeout = 500;

  unsigned char msgReceive[8]; // buffer for getting messages
  unsigned char msgGive[8]; // buffer for sending messages
  unsigned char len;

MCP_CAN CanBus(9);

void setup() {
  definePinModes();
  while(CAN_OK != CanBus.begin(CAN_500KBPS)) {
    Serial.println("CAN Bus is not operaitonal");
    delay(10);
  }
  startup(CanBus);
  BMS_timeout = millis() + BMS_timeout_limit;
  MC_timeout = millis() + MC_timeout_limit;
  ar_timeout = millis() + ar_timeout_limit;
  EVDC_timeout = millis() + BMS_timeout_limit;

}

void loop() {
  // code structure: do error checks first, then do other things

  while(CAN_MSGAVAIL == CanBus.checkReceive() {
    CAN.readMsgBuf(&len, msgReceive);
    switch(CAN.getCanId()) {
      case BMS::Message_1:
        float temp = BMS::getTemp(msgReceive);
        float voltage = BMS::getVoltage(msgRecieve);
        float current = BMS::getCurrent(msgReceive);
        BMS::timeout = millis() + BMS_timeout_limit;
        break;
      case BMS::Message_2:
        BMS::timeout = millis() + BMS_timeout_limit;
        break;
      case BMS::Message_3:
        BMS::timeout = millis() + BMS_timeout_limit;
        break;
      case EVDC::Message:
        int errors = EVDC::determineErrors(msgReceive);
        EVDC::timeout = millis() + EVDC_timeout_limit;
        break;
      case ar::Message:
        int errors = ar::determineErrors(msgReceive);
        ar::timeout = millis() + rear_timeout_limit;
        break;
      case MC::Message_1:
        float mctemp = MC::getTemp(msgRecieve);
        float phaseTemp = MC::getPhaseTemp(msgReceive);
        float mcSpeed = MC::getRPM(msgRecieve);
        float mcCurrent = MC::getCurrent(msgReceive);
        MC::timeout = millis() + MC_timeout_limit;
        break;
      case MC::Message_2:
        MC::timeout = millis() + MC_timeout_limit;
        break;
      case MC::Message_3:
        MC::timeout = millis() + MC_timeout_limit;
        break;
      default:
        break;
    }
  }
  if(millis() > BMS_timeout) {
    shutdownError(BMS_TIMED_OUT);
  }
  if(millis() > ar_timeout) {
    shutdownError(REAR_TIMED_OUT);
  }
  if(millis() > MC_timeout) {
    shutdownError(MC_TIMED_OUT);
  }
  if(millis() > EVDC_timeout) {
    shutdownError(EVDC_TIMED_OUT);
  }
  
}
