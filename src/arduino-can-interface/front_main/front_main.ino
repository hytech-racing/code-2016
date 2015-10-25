#inlcude <SPI.h>
#include "mcp_can.h"
#include mc.h
#include EVDC.h
#include BMS.h
#include ar.h
#include RPi.h
#include startupShutdown.h

#define BMS_TIMED_OUT 1
#define EVDC_TIMED_OUT 2
#define MC_TIMED_OUT 3
#define REAR_TIMED_OUT 4

MCP_CAN CanBus(9);

void setup() {
  definePinModes();
  while(CAN_OK != CanBus.begin(CAN_500KBPS)) {
    Serial.println("CAN Bus is not operaitonal");
    delay(10);
  }
  startup();
  BMS::timeout = millis() + BMS_timeout_limit;
  BMS::timeout = millis() + BMS_timeout_limit;
  BMS::timeout = millis() + BMS_timeout_limit;
  BMS::timeout = millis() + BMS_timeout_limit;

}

void loop() {
  // code structure: do error checks first, then do other things
  unsigned char msgReceive[8]; // buffer for getting messages
  unsigned char msgGive[8]; // buffer for sending messages
  unsigned char len;
  while(CAN_MSGAVAIL == CanBus.checkReceive() {
    CAN.readMsgBuf(&len, msgReceive);
    switch(CAN.getCanId()) {
      case BMS::Message_1:
        float temp = BMS::getTemp(msgReceive);
        float voltage = BMS::getVoltage(msgRecieve);
        float current = BMS::getCurrent(msgReceive);
        BMS::timeout = millis() + BMS_timeout_limit;
      case BMS::Message_2:
        BMS::timeout = millis() + BMS_timeout_limit;
      case BMS::Message_3:
        BMS::timeout = millis() + BMS_timeout_limit;
      case EVDC::Message:
        int errors = EVDC::determineErrors(msgReceive);
        EVDC::timeout = millis() + EVDC_timeout_limit;
      case ar::Message:
        int errors = ar::determineErrors(msgReceive);
        ar::timeout = millis() + rear_timeout_limit;
      case mc::Message_1:
        float mctemp = MC::getTemp(msgRecieve);
        float phaseTemp = MC::getPhaseTemp(msgReceive);
        float mcSpeed = MC::getRPM(msgRecieve);
        float mcCurrent = MC::getCurrent(msgReceive);
        MC::timeout = millis() + MC_timeout_limit;
      case mc::Message_2:
        MC::timeout = millis() + MC_timeout_limit;
      case mc::Message_3:
        MC::timeout = millis() + MC_timeout_limit;
      default:
    }
  }
  if(millis() > BMS_timeout_limit) {
    shutdownError(BMS_TIMED_OUT);
  }
  if(millis() > rear_timeout_limit) {
    shutdownError(REAR_TIMED_OUT);
  }
  if(millis() > MC_timeout_limit) {
    shutdownError(MC_TIMED_OUT);
  }
  if(millis() > EVDC_timeout_limit) {
    shutdownError(EVDC_TIMED_OUT);
  }
  
      
  
  // put your main code here, to run repeatedly:

}
