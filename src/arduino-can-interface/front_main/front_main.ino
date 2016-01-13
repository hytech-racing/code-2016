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

// global variables for data from CANBUS
float BMSstateOfCharge;
float BMScurrent;
float BMShighestTemp;
float BMSlowcellvoltage;

float MCphasetemp;
float MCmotortemp;
float MCrpm;
float MCcurrent;

int EVDCerror  = 0;

int ARerror = 0;


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
  // the basic premise is that I get the data from the CANBUS and
  // load it into variables. Then, I check the data for plausibility

  while(CAN_MSGAVAIL == CanBus.checkReceive() {
    CAN.readMsgBuf(&len, msgReceive);
    switch(CAN.getCanId()) {
      case BMS::Message_1:
        BMSstateOfCharge = BMS::getStateOfCharge(msgRecieve);
        BMS::timeout = millis() + BMS_timeout_limit;
        break;
      case BMS::Message_2:
        BMScurrent = BMS::getCurrent(msgRecieve);
      
        BMS::timeout = millis() + BMS_timeout_limit;
        break;
      case BMS::Message_3:
        
        BMS::timeout = millis() + BMS_timeout_limit;
        break;
      case BMS::Message_4:
        BMShighestTemp = BMS::getHighTemp(msgRecieve);
        BMS::timeout = millis() + BMS_timeout_limit;
        break;
      case BMS::Message_5:
        BMSlowcellvoltage = BMS::getLowVoltage(msgRecieve);
        BMS::timeout = millis() + BMS_timeout_limit;
        break;
      case EVDC::Message:
        EVDCerrors = EVDC::determineErrors(msgReceive);
        EVDC::timeout = millis() + EVDC_timeout_limit;
        break;
      case AR::Message:
        ARerror = ar::getError(msgReceive);
        ar::timeout = millis() + rear_timeout_limit;
        break;
      case MC::Message_Phase_temp:
        //MCmotortemp = MC::getMotorTemp(msgRecieve);
        MCphasetemp = MC::getPhaseTemp(msgReceive);
        //MCrpm = MC::getRPM(msgRecieve);
        //MCcurrent = MC::getCurrent(msgReceive);
        MC::timeout = millis() + MC_timeout_limit;
        break;
      case MC::Message_Motor_Temp:
        MCmotortemp = MC::getMotorTemp(msgRecieve);
        MC::timeout = millis() + MC_timeout_limit;
        break;
      case MC::Message_Motor_Speed:
        MCrpm = MC::getRPM(msgRecieve);
        MC::timeout = millis() + MC_timeout_limit;
        break;
      case MC::Message_Motor_Current:
        MCcurrent = MC::getCurrent(msgRecieve);
        MC::timeout = millis() + MC_timeout_limit;
        break;
      default:
        break;
    }
  }
  if(millis() > BMS_timeout) {
    shutdownError(BMS_TIMED_OUT, CanBus);
  }
  if(millis() > ar_timeout) {
    shutdownError(REAR_TIMED_OUT, CanBus);
  }
  if(millis() > MC_timeout) {
    shutdownError(MC_TIMED_OUT, CanBus);
  }
  if(millis() > EVDC_timeout) {
    shutdownError(EVDC_TIMED_OUT, CanBus);
  }
  
  // error checking
  
  if(BMSstateOfCharge < 10) {
   alertError(LOW_SOC);
  }
  if(BMSstateOfCharge < 5) {
   shutdownError(VERY_LOW_SOC)
  }
  
  if(BMShighestTemp > 80) {
   shutdownError(HIGH_BATT_TEMP);
  }
  if(MCphasetemp > 100) {
   shutdownError(HIGH_PHASE_TEMP);
  }
  if(MCmotortemp > 100) {
   shutdownError(HIGH_MOTOR_TEMP);
  }
  if(EVDCerror > 0) {
   shutdownError(EVDC_BASE_ERROR + EVDCerror);
  }
  if(ARerror > 0) {
   shutdownError(AR_BASE_ERROR + ARerror);
  }
  
  
}
