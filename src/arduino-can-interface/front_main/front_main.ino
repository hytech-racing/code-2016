
// list of shortcuts: pins not properly defined. startup sequence might be mess

#include <SPI.h>
#include <EEPROM.h>
#include "pinAndErrorDefinitions.h"
#include "mcp_can.h"
#include "mc.h"
#include "evdc.h"
#include "bms.h"
#include "ar.h"
#include "pi.h"
#include "shutdown.h"
#include "startup.h"
#include "buttons.h"

//#include "IMD.h"     we are going to go off of a digital read of OKHS instead

#define DEBUG_ACTIVATE 42 // declare to be 42 to turn on debug functionality



long CANtimer;

long BMS_timeout;
long BMS_timeout_limit = 2000; // 2000 ms
char BMS_Message_Checker; // these message checkers ensure that the computer processes at least one message of each type in the allotted time

long ar_timeout;
long ar_timeout_limit = 2000;

long MC_timeout;
long MC_timeout_limit = 1000;
char MC_Message_Checker;

long EVDC_timeout;
long EVDC_timeout_limit = 2000;

// global variables for data from CANBUS
float BMSwholePackVoltage = 300;
float BMSstateOfCharge = 50.0;
float BMScurrent = 0.0;
float BMShighestTemp = 20.0;
float BMSlowcellvoltage = 3.7;
float BMScurrentLimitKW = 100;
float BMS_low_cell_cutoff = 3.0;

float MCphasetemp = 20.0;
float MCmotortemp = 20.0;
float MCrpm = 0.0;
float MCcurrent = 0.0;

int EVDCerror  = 0;
int EVDCbuttons = 0;

int ARerror = 0;

int IMDerror = 0;
long IMDtimer = 0;


  unsigned char msgReceive[8]; // buffer for getting messages
  unsigned char msgGive[8]; // buffer for sending messages
  unsigned char len;

MCP_CAN CanBus(9);


void setup() {
  Serial.begin(9600);
  defineAndSetPinModes();
  while(CAN_OK != CanBus.begin(CAN_500KBPS)) {
    Serial.println("CAN Bus is not operaitonal");
    delay(10);
  }
  if(DEBUG_ACTIVATE != 42) {
    startupSequence(CanBus);
  }
  else {
    startupDebug(CanBus);
  }
  EEPROM.write(0, 0x00);
  BMS_timeout = millis() + BMS_timeout_limit;
  MC_timeout = millis() + MC_timeout_limit;
  ar_timeout = millis() + ar_timeout_limit;
  EVDC_timeout = millis() + BMS_timeout_limit;
  IMDtimer = millis() + 1000;

}

void loop() {
  
  // the basic premise is that I get the data from the CANBUS and
  // load it into variables. Then, I check the data for plausibility
  
  Serial.println("loop start");
  RPi::giveProgression(CanBus, 5);
  CANtimer = millis() + 100;
  while(CANtimer > millis()) { // the loop listens to CAN for 100 ms in order to make sure all messages are read
    while(CAN_MSGAVAIL == CanBus.checkReceive()) {
      CanBus.readMsgBuf(&len, msgReceive);
      switch(CanBus.getCanId()) {
        //BMS MESSAGES  BMS MESSAGES  BMS MESSAGES  BMS MESSAGES  BMS MESSAGES  BMS MESSAGES  BMS MESSAGES  BMS MESSAGES  
        case BMS::Message_1:
          BMSstateOfCharge = BMS::getStateOfCharge(msgReceive);
          BMS_Message_Checker |= 0x01;
          break;
        case BMS::Message_2:
          BMScurrent = BMS::getCurrent(msgReceive);
          BMScurrentLimitKW = BMS::getPackDCL(msgReceive);
          BMS_Message_Checker |= 0x02;   
          break;
        case BMS::Message_3:
          BMSwholePackVoltage = BMS::getPackVoltage(msgReceive);
          BMS_Message_Checker |= 0x04; 
          break;
        case BMS::Message_4:
          BMShighestTemp = BMS::getHighTemp(msgReceive);
          BMS_Message_Checker |= 0x08; 
          break;
        case BMS::Message_5:
          BMSlowcellvoltage = BMS::getLowVoltage(msgReceive);
          BMS_Message_Checker |= 0x10; 
          break;
          //EVDC and REAR MESSAGES  EVDC and REAR MESSAGES  EVDC and REAR MESSAGES  EVDC and REAR MESSAGES  EVDC and REAR MESSAGES  
        case EVDC::Message:
          EVDCerror = EVDC::getError(msgReceive);
          EVDCbuttons = EVDC::getButtons(msgReceive);
          EVDC_timeout = millis() + EVDC_timeout_limit;
          break;
        case AR::Message:
          ARerror = AR::getError(msgReceive);
          ar_timeout = millis() + ar_timeout_limit;
          break;
          //MOTOR CONTROLLER MESSAGES  MOTOR CONTROLLER MESSAGES  MOTOR CONTROLLER MESSAGES  MOTOR CONTROLLER MESSAGES
        case MC::Message_Phase_Temp:
          //MCmotortemp = MC::getMotorTemp(msgReceive);
          MCphasetemp = MC::getMaxPhaseTemp(msgReceive);
          //MCcurrent = MC::getCurrent(msgReceive);
          MC_Message_Checker |= 0x01; 
          break;
        case MC::Message_Motor_Temp:
          MCmotortemp = MC::getMotorTemp(msgReceive);
          MC_Message_Checker |= 0x02; 
          break;
        case MC::Message_Motor_Speed:
          MCrpm = MC::getMotorRPM(msgReceive);
          MC_Message_Checker |= 0x04; 
          break;
        case MC::Message_Motor_Current:
          MCcurrent = MC::getDCBusCurrent(msgReceive);
          MC_Message_Checker |= 0x08;
          break;
        default:
          break;
      }
    } // end "while canbus message available"
  } // end "while timer greater than millis()"
  
  // TIMER CHECKING  TIMER CHECKING  TIMER CHECKING  TIMER CHECKING  TIMER CHECKING  TIMER CHECKING  TIMER CHECKING  
  
  
  if(BMS_Message_Checker == 0x1F) { // all messages = 1 + 2 + 4 + 8 + 16
    BMS_timeout = millis() + BMS_timeout_limit;
    BMS_Message_Checker = 0;
  }
  
  if(MC_Message_Checker == 0x0F) { // all messages = 1 + 2 + 4 + 8
    MC_timeout = millis() + MC_timeout_limit;
    MC_Message_Checker = 0;
  }
  
  if(millis() > BMS_timeout) {
    shutdownError(CanBus, BMS_TIMED_OUT);
    BMS_Message_Checker = 0;
  }
  if(millis() > ar_timeout) {
    shutdownError(CanBus, AR_TIMED_OUT); 
  }
  if(millis() > MC_timeout) {
    shutdownError(CanBus, MC_TIMED_OUT);
    MC_Message_Checker = 0;
  }
  if(millis() > EVDC_timeout) {
    shutdownError(CanBus, EVDC_TIMED_OUT);
  }
  
  //ERROR CHECKING  ERROR CHECKING  ERROR CHECKING  ERROR CHECKING  ERROR CHECKING  ERROR CHECKING  
 
  
  if(digitalRead(IMDpin)) {
   IMDerror = 1;
  }
  
  Serial.print("StateOfCharge ");
  Serial.println(BMSstateOfCharge);
  
  if(BMSstateOfCharge < 10) {
   alertError(CanBus, LOW_SOC);
  }
  if(BMSstateOfCharge < 5) {
   shutdownError(CanBus, VERY_LOW_SOC);
  }
  if(BMShighestTemp > 80) {
   shutdownError(CanBus, HIGH_BATT_TEMP);
  }
  if(BMSlowcellvoltage < BMS_low_cell_cutoff) {
   shutdownError(CanBus, CELL_CRITICAL_LOW);
  }
  if(BMScurrent > (BMScurrentLimitKW*1000.0)/BMSwholePackVoltage) {
   shutdownError(CanBus, TOO_MUCH_CURRENT);
  }
  
  if(MCphasetemp > 100) {
   shutdownError(CanBus, HIGH_PHASE_TEMP);
  }
  if(MCmotortemp > 100) {
   shutdownError(CanBus, HIGH_MOTOR_TEMP);
  }
  
  if(EVDCerror > 1) {
   shutdownError(CanBus, EVDC_BASE_ERROR + EVDCerror);
  }
  
  if(EVDCerror == 1) {
    alertError(CanBus, EVDC_BASE_ERROR + EVDCerror);
    EVDC::goForLaunch(CanBus);
  }
  
  if(ARerror == 1) {
    alertError(CanBus, AR_BASE_ERROR + ARerror);
  
  if(ARerror > 1) {
   shutdownError(CanBus, AR_BASE_ERROR + ARerror);
  }
  
  if(IMDerror > 0){
    if(DEBUG_ACTIVATE != 42) {
      shutdownError(CanBus, IMD_BASE_ERROR + IMDerror);
    }
    else {
      alertError(CanBus, IMD_BASE_ERROR + IMDerror);
    }
  }
  
  if((EVDCbuttons & 0x01) == 0x01) { // if the shutdown button is pushed
   shutDownNormal(CanBus);
  }
  
}
