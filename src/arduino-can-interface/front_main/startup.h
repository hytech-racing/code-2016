//#include <SPI.h>
//#include "mcp_can.h"
//#include "mc.h"
//#include "evdc.h"
//#include "bms.h"
//#include "ar.h"
//#include "pi.h"
//#include "IMD.h"
/*
startup sequence according to Nathan

we only NEED to check for BMS fault if there was previously a BMS fault

5 buttons: 2 toggle, 2 momentary

3 lights: on, IMD, BMS

toggle ON led 

on startup, flash ON LED, quick on flash after brake+button press, precharge, flash start button after MC gets power, press start button again, start button has solid light, RTD sound, on LED solid, roll  

hold START to shutdown, turn off start LED, flash on LED

Pi Progression Statements:
0- dude ur in a dope car. this thing lit.
1- press progress to begin car startup sequence
2- Testing for BMS Error
3- press brake pedal and progress button to start car
4- Precharging the Caps
5- standard display
6- precharge complete. press start button to continue
7-
8- shutdown complete. Go Jackets!



*/

void startupSequence(MCP_CAN& lilEngineThatCAN) {
  unsigned char msgReceive[8];
  unsigned char len;
  boolean progressBlock = true;
  boolean button = false;
  boolean lightState;
  long progressionTimer = millis();

  for(int i = 0; i < 10; i++) {  // I want a delay for all the other systems to boot up.
    if(lightState) {
      startLightOn();
    }
    else {
      startLightOff();
    }
    lightState != lightState;
    RPi::giveProgression(lilEngineThatCAN,0);
    EVDC::calmTheFreakDown(lilEngineThatCAN);
    delay(100); 

  }
  
  while(progressBlock) { // wait for user input
    if(millis() > progressionTimer) {
      RPi::giveProgression(lilEngineThatCAN,1);
      EVDC::calmTheFreakDown(lilEngineThatCAN);
      
      if(lightState) {
      startLightOn();
      }
      else {
        startLightOff;
      }
      lightState != lightState;
 
      progressionTimer += 100;
    } 
    if(shutdownButton()) {
      progressBlock = false;
    }
  }
  
  progressBlock = true;
  

  if((EEPROM.read(0) & 0x01) == 1) { // for BMS error on last shutdown
    float BMSwholePackVoltage = 300;
    float BMSstateOfCharge = 50.0;
    float BMScurrent = 0.0;
    float BMShighestTemp = 20.0;
    float BMSlowcellvoltage = 3.7;
    float BMScurrentLimitKW = 100;
    float BMS_low_cell_cutoff = 2.0;
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
    
      if(BMS_Message_Checker == 0x1F) {  // if all messages have been seen
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
    EEPROM.write(0,0x00);
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
  
  
  
  
  button = 0;
  int brakes;
  while(progressBlock) { // wait for user input and brakes
    if(millis() > progressionTimer) {
      RPi::giveProgression(lilEngineThatCAN,3); 
      progressionTimer += 100;
    } 
    if(CAN_MSGAVAIL == lilEngineThatCAN.checkReceive()) {
      lilEngineThatCAN.readMsgBuf(&len, msgReceive);

      if(lilEngineThatCAN.getCanId() == EVDC::Message) {
        brakes = EVDC::getBrakes(msgReceive);
      }
      if(shutdownButton() && (brakes > 25)) { // if the progress button is pressed and brakes are more than 25%
        progressBlock == false;
      }
    }
  }
  startLightOn();
  delay(250);
  startLightOff();
  progressBlock = true;
  
  
  float DC_BUS_VOLT;
  digitalWrite(AirDCDC, HIGH);
  digitalWrite(software_shutdown_control, HIGH);
  digitalWrite(software_pushbutton_control, HIGH);
  while(progressBlock) {
    
    if(CAN_MSGAVAIL == lilEngineThatCAN.checkReceive()) {
      lilEngineThatCAN.readMsgBuf(&len, msgReceive);
      if(lilEngineThatCAN.getCanId() == EVDC::Message) {
        DC_BUS_VOLT = MC::getDCBusVoltage(msgReceive);
      }
      if(DC_BUS_VOLT > 250) { // if the progress button is pressed and brakes are more than 25%
        progressBlock == false;
      }
    }
  
    if(millis() > progressionTimer) {
      RPi::giveProgression(lilEngineThatCAN,4); 
      progressionTimer += 100;
    }
  }
  digitalWrite(software_pushbutton_control, LOW);
  progressBlock = true;
  
  
  while(progressBlock) { // wait for user input again
    if(millis() > progressionTimer) {
      RPi::giveProgression(lilEngineThatCAN,1); 
      progressionTimer += 100;
      if(lightState) {
      startLightOn();
      }
      else {
        startLightOff();
      }
      lightState != lightState;
    }
    if(shutdownButton()) {
      progressBlock = false;
    }
  }
  startLightOff();


  readyToDerpOn();
  for(int i = 0; i < 35; i++) { // play the ready to drive sound
    
    RPi::giveProgression(lilEngineThatCAN, 4);
    if(i > 30) {
        EVDC::goForLaunch(lilEngineThatCAN); // go for launch 3 seconds in
    }
    delay(100);
  }
  
  readyToDerpOff();
  
}






void startupDebug( MCP_CAN& lilEngineThatCAN) { //  safety checks? Pshhh
  RPi::giveProgression(lilEngineThatCAN,4);
  Serial.println("starting up DEBUG");
  Serial.println("starting up DEBUG");
  Serial.println("starting up DEBUG");
  digitalWrite(AIRdcdc, HIGH);  
  digitalWrite(software_pushbutton_control, HIGH);
  digitalWrite(software_shutdown_control, HIGH);
  readyToDerpOn();
  startLightOn();
  for(int i = 0; i < 35; i++) {
    RPi::giveProgression(lilEngineThatCAN,5);
    if(i > 30) {
      EVDC::goForLaunch(lilEngineThatCAN);
    }
    else{
      EVDC::calmTheFreakDown(lilEngineThatCAN);
    }
    delay(100);
  }
  readyToDerpOff();
  digitalWrite(software_shutdown_control, LOW);
  startLightOff();
  readyToDerpOff();
 
}
  





void defineAndSetPinModes() {
  
  pinMode(readyToDriveSound, OUTPUT);
  pinMode(AIRdcdc, OUTPUT);
  pinMode(software_shutdown_control, OUTPUT);
  pinMode(software_pushbutton_control, OUTPUT);
  pinMode(start_LED, OUTPUT);
  pinMode(fan_control, OUTPUT);
  pinMode(pump_control, OUTPUT);
  pinMode(IMD_LED, OUTPUT);
  pinMode(BMS_LED, OUTPUT);
  pinMode(MULTIPLEXER_SELECT_0, OUTPUT);
  pinMode(MULTIPLEXER_INPUT, INPUT);
  pinMode(10, OUTPUT); // CS pin
  pinMode(CONTROL_11, OUTPUT);
  pinMode(toggleButton2Pin, INPUT);
  pinMode(IMDpin, INPUT);
  pinMode(twenty_four_thermistor, INPUT);
  pinMode(MULTIPLEXER_SELECT_3, OUTPUT);
  pinMode(MULTIPLEXER_SELECT_2, OUTPUT);
  pinMode(MULTIPLEXER_SELECT_1, OUTPUT);

  
}
