#include <SPI.h>
#include "mcp_can.h"
#include "mc.h"
#include "evdc.h"
#include "bms.h"
#include "ar.h"
#include "pi.h"
#include "IMD.h"
//#include "shutdown.h"
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
  unsigned char msgReceive[8];
  unsigned char len;
  boolean progressBlock = true;
  int buttons = 0;
  long progressionTimer = millis();

  
  RPi::giveProgression(lilEngineThatCAN,0);
  EVDC::calmTheFreakDown(lilEngineThatCAN);
  delay(2000); // I want a delay for all the other systems to boot up.

  
  while(progressBlock) {
    if(millis() > progressionTimer) {
      RPi::giveProgression(lilEngineThatCAN,2);
       EVDC::calmTheFreakDown(lilEngineThatCAN);
 
      progressionTimer += 100;
    } 
    if(
  }
  
  progressBlock = true;
  

  if(EEPROM.read(0) == 1) { // for BMS error on last shutdown
    float BMSwholePackVoltage = 300;
    float BMSstateOfCharge = 50.0;
    float BMScurrent = 0.0;
    float BMShighestTemp = 20.0;
    float BMSlowcellvoltage = 3.7;
    float BMScurrentLimitKW = 100;
    float BMS_low_cell_cutoff = 3.0;
    int BMS_Message_Checker = 0;
    while(progressBlock) {
      if(millis() > progressionTimer) {
        RPi::giveProgression(lilEngineThatCAN,2); 
        EVDC::calmTheFreakDown(lilEngineThatCAN);

        progressionTimer += 100;
      }
      if(CAN_MSGAVAIL == lilEngineThatCAN.checkReceive()) {
        lilEngineThatCAN.readMsgBuf(&len, msgReceive);
        switch(lilEngineThatCAN.getCanId()) {
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
        } // end switch
      } // end "if available can message"
    
      if(BMS_Message_Checker == 0x1F) {
        progressBlock = false;
      }
      
      if(BMSstateOfCharge < 10) {
       alertError(lilEngineThatCAN, LOW_SOC);
       progressBlock = true;
      }
      if(BMSstateOfCharge < 5) {
       shutdownError(lilEngineThatCAN, VERY_LOW_SOC);
       progressBlock = true;
      }
      if(BMShighestTemp > 80) {
       shutdownError(lilEngineThatCAN, HIGH_BATT_TEMP);
       progressBlock = true;
      }
      if(BMSlowcellvoltage < BMS_low_cell_cutoff) {
       shutdownError(lilEngineThatCAN, CELL_CRITICAL_LOW);
       progressBlock = true;
      }
      if(BMScurrent > (BMScurrentLimitKW*1000.0)/BMSwholePackVoltage) {
       shutdownError(lilEngineThatCAN, TOO_MUCH_CURRENT);
       progressBlock = true;
      }
    } // end "while progress block"
  } // end "if bms error last time"
  
  progressBlock = true;
  
  while(progressBlock) {
    if(digitalRead(IMDpin)) {
      progressBlock = false;
    }
    else {
      shutdownError(lilEngineThatCAN, IMD_BASE_ERROR);
    }
  }
  
  progressBlock = true;
  
  digitalWrite(AIRdcdc, HIGH);
  digitalWrite(precharge, HIGH);
  digitalWrite(discharge, LOW);
  long prechargeTimer = millis() + 3000;
  while(prechargeTimer > millis()) {
      if(millis() > progressionTimer) {
        RPi::giveProgression(lilEngineThatCAN,2); 
        progressionTimer += 100;
      }
  }
  
  RPi::giveProgression(lilEngineThatCAN, 3);
  buttons = 0;
  int brakes;
  while(progressBlock) {
    if(millis() > progressionTimer) {
      RPi::giveProgression(lilEngineThatCAN,3); 
      progressionTimer += 100;
    } 
    if(CAN_MSGAVAIL == lilEngineThatCAN.checkReceive()) {
      lilEngineThatCAN.readMsgBuf(&len, msgReceive);

      if(lilEngineThatCAN.getCanId() == 0xDC) {
        buttons = EVDC::getButtons(msgReceive);
        brakes = EVDC::getBrakes(msgReceive);
      }
      if(((buttons & 0x02) == 0x02) && (brakes > 25)) { // if the progress button is pressed and brakes are more than 25%
        progressBlock == false;
      }
    }
  }
  
  progressBlock = true;
  
  digitalWrite(precharge, LOW);
  digitalWrite(TSMasterRelay, HIGH);
  digitalWrite(readyToDriveSound, HIGH);
  long readyToDriveTimer = millis() + 3000;
  while(prechargeTimer > millis()) {
      if(millis() > progressionTimer) {
        RPi::giveProgression(lilEngineThatCAN,4); 
        progressionTimer += 100;
      }
  }
  digitalWrite(readyToDriveSound, LOW);
 
  EVDC::goForLaunch(lilEngineThatCAN);
  RPi::giveProgression(lilEngineThatCAN, 5);
}


void startupDebug( MCP_CAN& lilEngineThatCAN) { //  safety checks? Pshhh
  Serial.println("starting up DEBUG");
  Serial.println("starting up DEBUG");
  Serial.println("starting up DEBUG");
  digitalWrite(AIRdcdc, HIGH);  
  digitalWrite(precharge, HIGH);
  digitalWrite(discharge, LOW);
  delay(3000);
  digitalWrite(TSMasterRelay, HIGH); 
  digitalWrite(precharge, LOW);
  RPi::giveProgression(lilEngineThatCAN,5);
  EVDC::goForLaunch(lilEngineThatCAN);

}
  
//  const int AIRdcdc = 2;
//const int AIRdcdc = 2;
//const int readyToDriveSound = 2; // ready to derp sound?
//const int precharge = 2;
//const int discharge = 2;
//const int TSMasterRelay = 2;
//const int IMDpin = 3;

void defineAndSetPinModes() {
  
  pinMode(readyToDriveSound, OUTPUT);
  pinMode(AIRdcdc, OUTPUT);
  pinMode(software_shutdown_control, OUTPUT);
  pinMode(software_puchbutton_control, OUTPUT);
  pinMode(start_LED, OUTPUT);
  pinMode(fan_control, OUTPUT);
  pinMode(pump_control, OUTPUT);
  pinMode(IMD_LED, OUTPUT);
  pinMode(BMS_LED, OUTPUT);
  pinMode(MULTIPLEXER_SELECT_0, OUTPUT);
  pinMode(MULTIPLEXER_INPUT, INPUT);
  pinMode(CONTROL_11, OUTPUT);
  pinMode(CONTROL_10, OUTPUT);
  pinMode(IMDpin, INPUT);
  pinMode(five_supply_check_2, INPUT);
  pinMode(MULTIPLEXER_SELECT_3, OUTPUT);
  pinMode(MULTIPLEXER_SELECT_2, OUTPUT);
  pinMode(MULTIPLEXER_SELECT_1, OUTPUT);

  
}
