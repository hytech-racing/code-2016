#include <SPI.h>
#include <EEPROM.h>
#include "pinDefinitions.h"
#include "mcp_can.h"
#include "mc.h"
#include "evdc.h"
#include "bms.h"
#include "ar.h"
#include "pi.h"
#include "startup.h"
#include"shutdown.h"
#include "IMD.h"

#define BMS_TIMED_OUT 1
#define EVDC_TIMED_OUT 2
#define MC_TIMED_OUT 3
#define AR_TIMED_OUT 4
#define LOW_SOC 5
#define VERY_LOW_SOC 6
#define HIGH_BATT_TEMP 7
#define LOW_CELL_CUTOFF 8
#define CELL_CRITICAL_LOW 9
#define TOO_MUCH_CURRENT 10
#define HIGH_PHASE_TEMP 11
#define HIGH_MOTOR_TEMP 12
#define EVDC_BASE_ERROR 13 // 13 to 18
#define AR_BASE_ERROR 19 // 19 to 25
#define IMD_BASE_ERROR 26


long BMS_timeout;
long BMS_timeout_limit = 2000; // 2000 ms
long ar_timeout;
long ar_timeout_limit = 2000;
long MC_timeout;
long MC_timeout_limit = 500;
long EVDC_timeout;
long EVDC_timeout_limit = 500;

// global variables for data from CANBUS
float BMSstateOfCharge;
float BMScurrent;
float BMShighestTemp;
float BMSlowcellvoltage;
float BMScurrentLimit;

float MCphasetemp;
float MCmotortemp;
float MCrpm;
float MCcurrent;

int EVDCerror  = 0;

int ARerror = 0;

int EVDCbuttons;

int IMDerror = 0;
int IMDtimer = 0;


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
  startupSequence(CanBus);
  EEPROM.write(0, 0x00);
  BMS_timeout = millis() + BMS_timeout_limit;
  MC_timeout = millis() + MC_timeout_limit;
  ar_timeout = millis() + ar_timeout_limit;
  EVDC_timeout = millis() + BMS_timeout_limit;

}

void loop() {
  // the basic premise is that I get the data from the CANBUS and
  // load it into variables. Then, I check the data for plausibility

  while(CAN_MSGAVAIL == CanBus.checkReceive()) {
    CanBus.readMsgBuf(&len, msgReceive);
    switch(CanBus.getCanId()) {
      case BMS::Message_1:
        BMSstateOfCharge = BMS::getStateOfCharge(msgReceive);
        BMS_timeout = millis() + BMS_timeout_limit;
        break;
      case BMS::Message_2:
        BMScurrent = BMS::getCurrent(msgReceive);
        BMScurrentLimit = BMS::getPackDCL(msgReceive);
        BMS_timeout = millis() + BMS_timeout_limit;
        break;
      case BMS::Message_3:
        
        BMS_timeout = millis() + BMS_timeout_limit;
        break;
      case BMS::Message_4:
        BMShighestTemp = BMS::getHighTemp(msgReceive);
        BMS_timeout = millis() + BMS_timeout_limit;
        break;
      case BMS::Message_5:
        BMSlowcellvoltage = BMS::getLowVoltage(msgReceive);
        BMS_timeout = millis() + BMS_timeout_limit;
        break;
      case EVDC::Message:
        EVDCerror = EVDC::getError(msgReceive);
        EVDCbuttons = EVDC::getButtons(msgReceive);
        EVDC_timeout = millis() + EVDC_timeout_limit;
        break;
      case AR::Message:
        ARerror = AR::getError(msgReceive);
        ar_timeout = millis() + ar_timeout_limit;
        break;
      case MC::Message_Phase_Temp:
        //MCmotortemp = MC::getMotorTemp(msgReceive);
        MCphasetemp = MC::getMaxPhaseTemp(msgReceive);
        //MCrpm = MC::getRPM(msgReceive);
        //MCcurrent = MC::getCurrent(msgReceive);
        MC_timeout = millis() + MC_timeout_limit;
        break;
      case MC::Message_Motor_Temp:
        MCmotortemp = MC::getMotorTemp(msgReceive);
        MC_timeout = millis() + MC_timeout_limit;
        break;
      case MC::Message_Motor_Speed:
        MCrpm = MC::getMotorRPM(msgReceive);
        MC_timeout = millis() + MC_timeout_limit;
        break;
      case MC::Message_Motor_Current:
        MCcurrent = MC::getDCBusCurrent(msgReceive);
        MC_timeout = millis() + MC_timeout_limit;
        break;
      default:
        break;
    }
  }
  if(millis() > BMS_timeout) {
    shutdownError(CanBus, BMS_TIMED_OUT);
  }
  if(millis() > ar_timeout) {
    shutdownError(CanBus, AR_TIMED_OUT);  }
  if(millis() > MC_timeout) {
    shutdownError(CanBus, MC_TIMED_OUT);
  }
  if(millis() > EVDC_timeout) {
    shutdownError(CanBus, EVDC_TIMED_OUT);
  }
  
  IMDtimer++;
  if(IMDtimer > 10) {
   IMDtimer = 0;
   IMDerror = IMD::checkError();
  }
  // error checking
  
  if(BMSstateOfCharge < 10) {
   alertError(CanBus, LOW_SOC);
  }
  if(BMSstateOfCharge < 5) {
   shutdownError(CanBus, VERY_LOW_SOC);
  }
  if(BMShighestTemp > 80) {
   shutdownError(CanBus, HIGH_BATT_TEMP);
  }
  if(BMSlowcellvoltage < LOW_CELL_CUTOFF) {
   shutdownError(CanBus, CELL_CRITICAL_LOW);
  }
  if(BMScurrentLimit > BMScurrent) {
   shutdownError(CanBus, TOO_MUCH_CURRENT);
  }
  
  if(MCphasetemp > 100) {
   shutdownError(CanBus, HIGH_PHASE_TEMP);
  }
  if(MCmotortemp > 100) {
   shutdownError(CanBus, HIGH_MOTOR_TEMP);
  }
  
  if(EVDCerror > 0) {
   shutdownError(CanBus, EVDC_BASE_ERROR + EVDCerror);
  }
  
  if(ARerror > 0) {
   shutdownError(CanBus, AR_BASE_ERROR + ARerror);
  }
  
  if(IMDerror > 0){
	shutdownError(CanBus, IMD_BASE_ERROR + IMDerror);
  }
  
  if((EVDCbuttons & 0x01) == 0x01) { // if the shutdown button is pushed
   shutDownNormal(CanBus);
  }
  
}
