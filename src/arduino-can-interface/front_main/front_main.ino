// list of shortcuts: startup sequence might be mess, flash start LED when starting up, wait for MC line voltage to stop RTD sound, quick flash after brake+button press, precharge, flash start button after MC gets power, press start button again, start button has solid light, RTD sound,  



#define DEBUG_ACTIVATE 42 // declare to be 42 to turn on debug functionality, 203 for board test
/*
Debug mode does the following things:
skips startup sequence
does not turn off AIRs in case of an error - turns on RTD sound instead for a few seconds
does not stop all car functionality


*/

#define PRINT_MODE 64 // declare to be 64 to turn on printing 
#define PRINT_DELAY 300 // 1 second between prints
/* 
print mode does the following things:

sets serial rate to 115200 bits per second

prints several variables once every second, including:

motor temperature
IGBT temperature
BMS pack voltage
BMS state of charge
BMS current 
highest cell temperature
BMS current limit (in actual amps)
button states
EVDC error
Rear arduino error
IMD state
various thermistor temperatures



*/

#include <SPI.h>
#include <EEPROM.h>
// include general logic files
#include "pinAndErrorDefinitions.h"
//#include "multiplexer.h" moved to misc functions
#include "thermistor.h"
// include files for car functions
#include "mcp_can.h"
#include "miscFunctions.h"
#include "mc.h"
#include "evdc.h"
#include "bms.h"
#include "ar.h"
#include "pi.h"
#include "shutdown.h"
#include "startup.h"


//#include "IMD.h"     we are going to go off of a digital read of OKHS instead






long printTimer;

long CANtimer;
#define MESSAGE_ACQUIRE_DELAY 200

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
float BMS_low_cell_cutoff = 2.0;

float MCphasetemp = 20.0;
float MCmotortemp = 20.0;
float MCrpm = 0.0;
float MCcurrent = 0.0;

int EVDCerror  = 0;

int ARerror = 0;

int IMDerror = 0;
long IMDtimer = 0;

int shutdownCounter = 0;


  unsigned char msgReceive[8]; // buffer for getting messages
  unsigned char msgGive[8]; // buffer for sending messages
  unsigned char len;

MCP_CAN CanBus(10);














void setup() {
  if(PRINT_MODE == 64) {
    Serial.begin(115200);
  }
  else {
    Serial.begin(9600);
  }
    
  defineAndSetPinModes(); // found in startup.h
  
  if(DEBUG_ACTIVATE == 203){
    testMicroBoard(CanBus);
  } 
  
  while(CAN_OK != CanBus.begin(CAN_500KBPS)) {
    Serial.println("CAN Bus is not operaitonal");
    delay(10);
    BMSlightOn();
    IMDlightOn();
  }
  
  BMSlightOff();
  IMDlightOff();
  
  if(DEBUG_ACTIVATE == 42) {
    startupDebug(CanBus);
  }
  else{
    startupSequence(CanBus);
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
  if(PRINT_MODE != 64){
    Serial.println("loop start");
  }
  RPi::giveProgression(CanBus, 5);
  EVDC::goForLaunch(CanBus);
  CANtimer = millis() + MESSAGE_ACQUIRE_DELAY;
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
          EVDC_timeout = millis() + EVDC_timeout_limit;
          break;
        case AR::Message:
          ARerror = AR::getError(msgReceive);
          ar_timeout = millis() + ar_timeout_limit;
          break;
          //MOTOR CONTROLLER MESSAGES  MOTOR CONTROLLER MESSAGES  MOTOR CONTROLLER MESSAGES  MOTOR CONTROLLER MESSAGES
        case MC::Message_Phase_Temp:
          MCmotortemp = MC::getMotorTemp(msgReceive);
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
 
  
  if(!digitalRead(IMDpin)) {
   IMDerror = 1;
  }
  
  
//  int coolantThermistor1 = getMultiplexerAnalog(COOLANT_SENSOR_1_SELECT);
//  int coolantThermistor2 = getMultiplexerAnalog(COOLANT_SENSOR_2_SELECT);
//  int coolantThermistor3 = getMultiplexerAnalog(COOLANT_SENSOR_3_SELECT);
//  int highCoolantTemp = max(checkThermistor(10000, coolantThermistor1), checkThermistor(10000, coolantThermistor2));
//  highCoolantTemp = max(highCoolantTemp, checkThermistor(10000, coolantThermistor3));
//  
  
  
//  
//  if(highCoolantTemp > MAXIMUM_COOLANT_TEMPERATURE) {
//    shutdownError(CanBus, COOLANT_OVER_TEMP);
//  }
//  else if(highCoolantTemp > COOLANT_TEMPERATURE_WARNING) {
//    alertError(CanBus, COOLANT_WARNING_TEMP);
//  }
  
  int twelveThermistor2 = getMultiplexerAnalog(TWELVE_THERMISTOR2_SELECT);
  int twelveThermistor1 = getMultiplexerAnalog(TWELVE_THERMISTOR1_SELECT);
  long highTwelveTemp = max(checkThermistor(10000, twelveThermistor2), checkThermistor(10000, twelveThermistor2));
  if(highTwelveTemp > MAX_DCDC_TEMP) {
    shutdownError(CanBus, TWELVE_OVER_TEMP);
  }
  else if(highTwelveTemp > WARNING_DCDC_TEMP) {
    alertError(CanBus, TWELVE_WARNING_TEMP);
  }
  
  int fiveTemp = checkThermistor(10000, getMultiplexerAnalog(FIVE_THERMISTOR_SELECT));
  if(fiveTemp > MAX_DCDC_TEMP) {
    shutdownError(CanBus, FIVE_OVER_TEMP);
  }
  else if(fiveTemp > WARNING_DCDC_TEMP) {
    alertError(CanBus, FIVE_WARNING_TEMP);
  }
  
  
  
  
  if(getMultiplexerAnalog(FIVE_SUPPLY_CHECK_SELECT) < 800) {
    alertError(CanBus, FIVE2_VOLTAGE_DIP);
  }
  
  
  
  
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
  
  int highCoolantSystemTemp = max(MCphasetemp, MCmotortemp);
  int fanAndPumpDuty = map(highCoolantSystemTemp, 20, 60, 64, 255); //full power at 60 degrees C, quarter power at 20 degrees
  if(fanAndPumpDuty > 255){
    fanAndPumpDuty = 255;
  }
  if(fanAndPumpDuty < 0) {
    fanAndPumpDuty = 0;
  }
  analogWrite(fan_control, fanAndPumpDuty);
  analogWrite(pump_control, fanAndPumpDuty); // got myself a bit of a proportional controller here
  
  
  if((EVDCerror & 0x01) == 0x01) {
    alertError(CanBus, EVDC_PEDAL_ERROR);
  }
  
  if(ARerror == 1 || ARerror == 2) {
    shutdownError(CanBus, AR_BASE_ERROR + ARerror);
  }
  else if(ARerror > 2) {
   alertError(CanBus, AR_BASE_ERROR + ARerror);
  }
  
  if(IMDerror > 0){
      shutdownError(CanBus, IMD_BASE_ERROR); 
  }
  
  if(shutdownButton()) { // if the shutdown button is pushed for about 1 second
    shutdownCounter++;
    if(shutdownCounter > 30) {
      //shutDownNormal(CanBus);
    }
  }
  else {
    shutdownCounter = 0;
  }
  
  
  //PRINTING STUFF***************************************************************************************************************
  if((PRINT_MODE == 64) && (millis() > printTimer)) {
    printTimer = millis() + PRINT_DELAY;
    Serial.println(" ");
    Serial.println(" ");
    Serial.println(" ");
    Serial.println("igbt and motor temps");
    Serial.println(MCphasetemp);
    Serial.println(MCmotortemp);
    Serial.println("BMS data");
    Serial.print(BMSwholePackVoltage);
    Serial.println(" volts, pack");
    Serial.print(BMSstateOfCharge);
    Serial.println(" percent");
    Serial.print(BMScurrent);
    Serial.println(" amps");
    Serial.print(BMShighestTemp);
    Serial.println(" degrees C, battery");
    Serial.print(BMSlowcellvoltage);
    Serial.println(" volts, low cell");
    Serial.print(BMScurrentLimitKW);
    Serial.println(" amps, limit");
    Serial.print("rear error: ");
    Serial.println(ARerror);
    Serial.print("EVDC error: ");
    Serial.println(EVDCerror);
    Serial.println("shutdown cycle boost toggle");
    if(shutdownButton()) {
      Serial.println("1 ");
    }
    else {
      Serial.println("0 ");
    }
    
    if(boostButton()) {
      Serial.println("1 ");
    }
    else {
      Serial.println("0 ");
    }
    
    if(cycleButton()) {
      Serial.println("1 ");
    }
    else {
      Serial.println("0 ");
    }
    
    if(toggleButton()) {
      Serial.println("1 ");
    }
    else {
      Serial.println("0 ");
    }
    
    Serial.print("IMD: ");
    Serial.println(analogRead(IMDpin));
    
    Serial.print(highTwelveTemp);
    Serial.println(" degrees C, 12v DCDC");
    Serial.print(fiveTemp);
    Serial.println(" degrees C, 5v DCDC");
//    Serial.println("coolant 1,2,3 raw, high");
//    Serial.println(coolantThermistor1);
//    Serial.println(coolantThermistor2);
//    Serial.println(coolantThermistor3);
//    Serial.print(highCoolantTemp);
//    Serial.println(" degrees C");
    Serial.println(" ");
    Serial.println(" ");
    Serial.println(" ");
    
  } // end "if print mode on"
    

} // end of void loop
