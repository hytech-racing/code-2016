#define DEBUG_ACTIVATE 0 // declare to be 42 to turn on debug functionality, 203 for board test

/*
Debug mode does the following things:
skips startup sequence
does not turn off AIRs in case of an error - turns on RTD sound instead for a few seconds
does not stop all car functionality
*/

#include <SPI.h>
#include <EEPROM.h>
#include "pinAndErrorDefinitions.h"
#include "mcp_can.h"
#include "miscFunctions.h"
#include "mc.h"
#include "evdc.h"
#include "bms.h"
#include "ar.h"
#include "pi.h"


boolean extendedScan = true; // this needs to be up here
boolean firstLoop = true;
long RTDEndTime;
long BRBprechargeEndTime;
#define readyToDriveOnTime 2500

#include "shutdown.h"
#include "startup.h"


long BMS_timeout;
long BMS_timeout_limit = 4000; // 2000 ms

long ar_timeout;
long ar_timeout_limit = 4000;

// global variables for data from CANBUS
unsigned char BMSrelayStatus = 1;


float MCphasetemp = 24.2;
float MCmotortemp = 24.2;
float MCrpm = 0.0;
float MCcurrent = 0.0;
float MCbusVoltage = 242;
boolean MCisActivated = false;
boolean BRBpressed = false;

int EVDCerror = 0;
int EVDCstate = 0;
int EVDCbrake = 42;

int ARvoltage = 25;
int ARerror = 0;

int IMDerror = 0;
long IMDtimer = 0;

int flashingIndicator = 0;

int oldFanDuty = 0;
int oldPumpDuty = 0;

boolean precharging = false;

unsigned char msgReceive[8]; // buffer for getting messages
unsigned char msgGive[8]; // buffer for sending messages
unsigned char len;

long CANtimer;
#define MESSAGE_ACQUIRE_DELAY 200
#define EXTENDED_SCAN_DELAY 1000

MCP_CAN CanBus(10);


void setup() { // in startup, I initialize the CAN chips and serial and call the startup sequence
  
  defineAndSetPinModes();
  
  Serial.begin(9600);
      
  if(DEBUG_ACTIVATE == 203){
    Serial.println("testing board");
    testMicroBoard(CanBus);
  } 
  
  while(CAN_OK != CanBus.begin(CAN_500KBPS)) {
    Serial.println("CAN Bus is not operaitonal");
    delay(10);
  }
 
  if(DEBUG_ACTIVATE == 42) {
    startupDebug(CanBus);
  }
  else {
    startupSequence(CanBus);
  }
  
  BMS_timeout = millis() + BMS_timeout_limit; // don't want premature timeouts
  ar_timeout = millis() + ar_timeout_limit;
}


































void loop() {
  
  // the basic premise is that I get the data from the CANBUS and
  // load it into variables. Then, I check the data for plausibility
   
  RPi::giveProgression(CanBus, 5);
  
  if(millis() > RTDEndTime){ // turn off ready to drive if it is time to do so
    readyToDerpOff();
  }
  
  if(extendedScan) { // if we are just starting up, then listen to CAN for 1 second, instead of only 200 ms
    CANtimer = millis() + EXTENDED_SCAN_DELAY;
    extendedScan = false;
  }
  else {
    CANtimer = millis() + MESSAGE_ACQUIRE_DELAY;
  }
  
  while(CANtimer > millis()) { // the loop listens to CAN for MESSAGE_ACQIRE_DELAY (in milliseconds) in order to make sure all messages are read
    while(CAN_MSGAVAIL == CanBus.checkReceive()) {
      CanBus.readMsgBuf(&len, msgReceive);
      switch(CanBus.getCanId()) {
          //BMS MESSAGES 
        case BMS::Message_1:
          BMSrelayStatus = BMS::getRelayStatus(msgReceive);
          BMS_timeout = millis() + BMS_timeout_limit;          
          break;
          //EVDC and REAR MESSAGES
        case EVDC::Message:
          EVDCerror = EVDC::getError(msgReceive);
          EVDCstate = EVDC::getLaunchedState(msgReceive);
          break;
        case AR::Message:
          ARerror = AR::getError(msgReceive);
          ARvoltage = AR::getVoltage(msgReceive);
          ar_timeout = millis() + ar_timeout_limit;
          break;
          //MOTOR CONTROLLER MESSAGES
        case MC::Message_Phase_Temp:
          MCphasetemp = MC::getMaxPhaseTemp(msgReceive);
          break;
        case MC::Message_Motor_Temp:
          MCmotortemp = MC::getMotorTemp(msgReceive);
          break;
        case MC::Message_Motor_A7:
          MCbusVoltage = MC::getDCBusVoltage(msgReceive);
          break;
      }
    } // end "while canbus message available"
  } // end "while timer greater than millis()"
  
  
  // TIMER CHECKING  
  
  
  if(millis() > BMS_timeout) {
    shutdownError(CanBus, BMS_TIMED_OUT);
  }
  if(millis() > ar_timeout) {
    shutdownError(CanBus, AR_TIMED_OUT); 
  }
  
  //ERROR CHECKING  
 
  
  if(analogRead(IMDpin) < 300) { // checking for IMD error here
   IMDerror = 1;
  }
  else{
    IMDerror = 0;
  }
  
  if((BMSrelayStatus & 0x01) == 0) { // bit 0 will be 1 if discharge is on, and 0 if it is off
    shutdownError(CanBus, BMS_TIMED_OUT);
  }
  
  // here I check for over-temps in the motor and controller.
  
  if(MCphasetemp > 100) { 
    shutdownError(CanBus, HIGH_PHASE_TEMP);
  }
  
  if(MCmotortemp > 100) {
    shutdownError(CanBus, HIGH_MOTOR_TEMP);
  }
  
  // here I check for a BRB pressed. If the voltage is less than 160 volts, then
  // I tell the EVDC to stop. I wait for the voltage to rise above 160 volts again,
  // then wait for a button press. I then wait a few seconds for the precharge to happen, then
  // send the EVDC a ready-to-go signal.
  
  if(MCbusVoltage < 160) { // if the voltage is less than 160
    BRBpressed = true;
    precharging = false;
    EVDC::calmTheFreakDown(CanBus, 74); // tell the EVDC to stop
  }
  else if(BRBpressed){ // if MC has voltage and BRB was pressed
    if(getMultiplexerAnalog(IMD_PWM_HIGH_SELECT) < 500){ // if button was pressed
      BRBprechargeEndTime = millis() + 500; // set the precharge end time
      flashingIndicator = 1-flashingIndicator;
      precharging = true;
      EVDC::calmTheFreakDown(CanBus, 76); // keep telling the EVDC to stop
    }
    else if((millis() > BRBprechargeEndTime) && precharging) { // if it is precharging
      RTDEndTime = millis() + 2500; // set the ready to drive end time
      readyToDerpOn();
      BRBpressed = false;
      precharging = false;
      EVDC::goForLaunch(CanBus); // tell the EVDC to go
    }
    else{ // if the button was not pressed yet, flash the start LED
      flashingIndicator = 1-flashingIndicator;
      EVDC::calmTheFreakDown(CanBus, 75);    
    }
    if(flashingIndicator > 0) {
      startLightOn();
    }
    else{
      startLightOff();
    }
  } // if everything is normal, turn on LED and launch the EVDC
  else {
      startLightOn();
      EVDC::goForLaunch(CanBus);
  }
  
  
  //pump works at about 220 pwm
  //fan works at about 110 pwm
  
  // here I take the coolant values and use those to adjust fan and pump speed
  
  int highCoolantSystemTemp = max(MCphasetemp, MCmotortemp);
  int desiredFanDuty = 0;
  int desiredPumpDuty = 0;
  
  
  if(highCoolantSystemTemp > 40) {
    desiredPumpDuty = map(highCoolantSystemTemp, 40, 55, 220, 255); // max pump speed at 55 degrees C
  }
  
  if(highCoolantSystemTemp > 55) {
    desiredFanDuty = map(highCoolantSystemTemp, 55, 70, 110, 255); // max fan speed at 70 degrees C
  }
  else{
    desiredFanDuty = 0;
  }
  
  if(desiredPumpDuty > 255) {
    desiredPumpDuty = 255;
  }
  if(desiredPumpDuty < 220) { // make the minimum pump speed to be 220
    desiredPumpDuty = 220;
  }
  
  if(desiredFanDuty > 255) {
    desiredFanDuty = 255;
  }
  if(desiredFanDuty < 0) {
    desiredFanDuty = 0;
  }

    
  if(desiredFanDuty > oldFanDuty) { // we have to ramp up the fan and pump speeds
    oldFanDuty++;
  }
  else if(desiredFanDuty < oldFanDuty) {
    oldFanDuty = desiredFanDuty;
  }
  
  if(desiredPumpDuty > oldPumpDuty) {
    oldPumpDuty++;
  }
  else if(desiredPumpDuty < oldPumpDuty) {
    oldPumpDuty = desiredPumpDuty;
  }
  
  analogWrite(fan_control, oldFanDuty);// write the PWM values
  analogWrite(pump_control, oldPumpDuty); 
  
  
  
  
  if(ARerror == 1) { // if the thermistors are too hot
   shutdownError(CanBus, AR_BASE_ERROR + ARerror);
  }
  
  if(ARvoltage < 20) { // if the LV batteries are less than 20 volts (2.5 volts per cell);
    shutdownError(CanBus, 21);
  }
  
  if(IMDerror == 1){ // if the IMD pin is low
    shutdownError(CanBus, IMD_BASE_ERROR); 
  }
  
} // end of void loop
