#include <SPI.h>
#include "mcp_can.h"
#include "mc.h"
#include "evdc.h"
#include "bms.h"
#include "ar.h"
#include "pi.h"
#include "IMD.h"
/*
startup sequence according to Nathan

we only NEED to check for BMS fault if there was previously a BMS fault


so, sequence is: car is switched on
asks for permission to go forward
checks BMS for errors if need be
checks IMD for errors
AIRs close
flips precharge
precharges
asks if they want to start the car
if brakes are on, the TSMS relay flips
RTD sound sounds
car is ready to drive
vroom vroom

Pi Progression Statements:
0- dude ur in a dope car. this thing lit.
1- press continue to begin car startup sequence
2- beginning startup checks
3- press brake pedal and continue button to start car
4- car is ready to drive! (while ready to drive sound is going off)
5- standard display



*/

void startupSequence(MCP_CAN& lilEngineThatCAN) { // 0 means a normal startup, 1 means BMS resset, 2 means IMD reset
  unsigned char msgRecieve[8];
  boolean progressBlock = true;
  int buttons = 0;
  
  RPi::giveProgression(0);
  delay(2000); // I want a delay for all the other systems to boot up.

  
  while((CAN_MSGAVAIL == CanBus.checkReceive()) && progressBlock) {
    RPi::giveProgression(1); 
    CanBus.readMsgBuf(&len, msgReceive);
    if(CanBus.getCanId() == 0xDC) {
      buttons = EVDC::getButtons(msgReceive);
    }
    if((buttons & 0x02) == 0x02) {
      brogressBlock == false;
    }
  }
  
  progressBlock = true;
  
  if(EEPROM.read(0) == 1) { // for BMS error on last shutdown
    while((CAN_MSGAVAIL == CanBus.checkReceive()) && progressBlock) {
    RPi::giveProgression(2); 
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
  




  digitalWrite(precharge, HIGH);
  delay(3000);
  digitalWrite(Air4, HIGH); // brake needs to be pressed in order to go into a ready-to-drive state
  digitalWrite(precharge, LOW);
  digitalWrite(readyToDriveSound, HIGH);
  delay(2000);
  digitalWrite(readyToDriveSound, LOW);
  EVDC::goForLaunch(lilEngineThatCAN);
}


void startupDebug( MCP_CAN& lilEngineThatCAN) { //  safety checks? Pshhh
  Serial.println("starting up DEBUG");
  Serial.println("starting up DEBUG");
  Serial.println("starting up DEBUG");
   digitalWrite(Air1, HIGH);
  delay(500);
  digitalWrite(Air2, HIGH);
  delay(500);
  digitalWrite(Air3, HIGH);  
  digitalWrite(precharge, HIGH);
  delay(3000);
  digitalWrite(Air4, HIGH); 
  digitalWrite(precharge, LOW);
  RPi::giveProgression(lilEngineThatCAN,5);
  EVDC::goForLaunch(lilEngineThatCAN);

}
  

void definePinModes() {
  pinMode(2, INPUT);
}
