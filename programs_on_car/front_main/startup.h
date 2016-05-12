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


  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  

boolean startupSequence(MCP_CAN& lilEngineThatCAN) {
  unsigned char msgReceive[8];
  unsigned char len;
  boolean progressBlock = true;
  boolean button = false;
  int lightState;
  long progressionTimer = millis();
  
  startLightOn();
  Serial.println("start light on top of sequence");
  delay(1000);



  for(int i = 0; i < 8; i++) {  // I want a delay for all the other systems to boot up.
    startLightOn();
    delay(100);
    startLightOff();
    delay(100); 
    RPi::giveProgression(lilEngineThatCAN,0);
    EVDC::calmTheFreakDown(lilEngineThatCAN, 65);
    delay(100); 

  }
  
  boolean beginningButtonState = boostButton();
  
  while(progressBlock) { // wait for user input
    if(millis() > progressionTimer) {
      RPi::giveProgression(lilEngineThatCAN,1);
      EVDC::calmTheFreakDown(lilEngineThatCAN, 66);
      
      if(lightState > 0) {
        startLightOn();
      }
      else {
        startLightOff();
      }
      lightState = 1- lightState;
 
      progressionTimer = millis() + 100;
    } 
    Serial.println("push button");
    if(getMultiplexerAnalog(IMD_PWM_HIGH_SELECT) < 50) {
      progressBlock = false;
    }
  }
  
  progressBlock = true;
  
/*
  if((EEPROM.read(0) & 0x01) == 1) { // for BMS error on last shutdown
    float BMSwholePackVoltage = 300;
    float BMSstateOfCharge = 50.0;
    float BMScurrent = 0.0;
    float BMShighestTemp = 20.0;
    float BMSlowcellvoltage = 3.65;
    float BMShighcellvoltage = 3.65;
    float BMScurrentLimitKW = 100;
    float BMS_low_cell_cutoff = 2.0;
    int BMS_Message_Checker = 0;
    
    digitalWrite(AirDCDC, LOW);
    digitalWrite(software_shutdown_control, LOW);
    digitalWrite(software_pushbutton_control, LOW);
    
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
       progressBlock = false;
      }
      if(BMSstateOfCharge < 5) {
       errorInStartup(lilEngineThatCAN, VERY_LOW_SOC);
       return false;
       progressBlock = true;
      }
      if(BMShighestTemp > 80) {
       errorInStartup(lilEngineThatCAN, HIGH_BATT_TEMP);
       return false;
       progressBlock = true;
      }
      if(BMSlowcellvoltage < BMS_low_cell_cutoff) {
       errorInStartup(lilEngineThatCAN, CELL_CRITICAL_LOW);
       return false;
       progressBlock = true;
      }
      if(BMScurrent > (BMScurrentLimitKW*1000.0)/BMSwholePackVoltage) {
       errorInStartup(lilEngineThatCAN, TOO_MUCH_CURRENT);
       return false;
       progressBlock = true;
      }
    } // end "while progress block"
    EEPROM.write(0,0x00);
  } // end "if bms error last time"
  */
  
 
  progressBlock = true;
  int counter = 0;
  
  Serial.println("IMD check");
  while(analogRead(IMDpin) < 300) {
    delay(100);
    EVDC::calmTheFreakDown(lilEngineThatCAN, 73);
    Serial.print("IMD: ");
    Serial.println(analogRead(IMDpin));
    counter++;
    if (counter > 50) { // 5 seconds for the IMD to turn on
      while(1) {
        IMDlightOn();
      }
    }
  }
  
  beginningButtonState = newBoostButton();
  
  button = 0;
  int brakes = 0;
  while(progressBlock) { // wait for user input and brakes
    if(millis() > progressionTimer) {
      RPi::giveProgression(lilEngineThatCAN,3); 
      EVDC::calmTheFreakDown(lilEngineThatCAN, 71);
      progressionTimer += 100;
    } 
    if(CAN_MSGAVAIL == lilEngineThatCAN.checkReceive()) {
      lilEngineThatCAN.readMsgBuf(&len, msgReceive);

      if(lilEngineThatCAN.getCanId() == EVDC::Message) {
        brakes = EVDC::getBrakes(msgReceive);
        if(brakes > 150) {
          brakes = 0;
        }
        Serial.println("brakes ");
        Serial.println(brakes);
      }
      if((getMultiplexerAnalog(IMD_PWM_HIGH_SELECT) < 50) && (brakes > 25)) { // if the progress button is pressed and brakes are more than 25%
        progressBlock = false;
        Serial.println("both conditions met");
        
      }
    }
  }
  
  
  
  startLightOn();
  delay(250);
  startLightOff();
  progressBlock = true;
  
  
  float DC_BUS_VOLT;
  Serial.end();
    pinMode(0, OUTPUT);
  pinMode(1, OUTPUT);
  pinMode(2, OUTPUT);
  //pinMode(3, OUTPUT);
  digitalWrite(0, LOW);
  
  digitalWrite(3, HIGH);
  
  
  delay(500);
  

  digitalWrite(1, HIGH);
  
  digitalWrite(2, HIGH);
  
  
  delay(500);
   
  
  while(progressBlock) {
    
    if(CAN_MSGAVAIL == lilEngineThatCAN.checkReceive()) {
      lilEngineThatCAN.readMsgBuf(&len, msgReceive);
      if(lilEngineThatCAN.getCanId() == MC::Message_Motor_A7) {
        DC_BUS_VOLT = MC::getDCBusVoltage(msgReceive);
       // Serial.println("motor controler voltage");
        //Serial.println(DC_BUS_VOLT);
      }
      if(DC_BUS_VOLT > 240) { // if the motor controller voltage is greater than 250 volts
        progressBlock = false;
      }
      
    }
  
    if(millis() > progressionTimer) {
      RPi::giveProgression(lilEngineThatCAN,4); 
      EVDC::calmTheFreakDown(lilEngineThatCAN, 68);
      progressionTimer += 100;
    }
  }
  digitalWrite(software_pushbutton_control, LOW);
  //Serial.println("pushbutton off");
  progressBlock = true;
  
  beginningButtonState = newBoostButton();
  
  while(progressBlock) { // wait for user input again
    if(millis() > progressionTimer) {
      RPi::giveProgression(lilEngineThatCAN,6);  // CHECK TO SEE IF WORKS
      EVDC::calmTheFreakDown(lilEngineThatCAN, 69);

      progressionTimer += 100;
      if(lightState) {
      startLightOn();
      }
      else {
        startLightOff();
      }
      lightState != lightState;
    }
    if(getMultiplexerAnalog(IMD_PWM_HIGH_SELECT) < 50) {
      progressBlock = false;
    }
  }
  startLightOff();
 


  progressBlock = true;
  int launchState = 0;
  
  while(progressBlock){
    if(CAN_MSGAVAIL == lilEngineThatCAN.checkReceive()) {
      lilEngineThatCAN.readMsgBuf(&len, msgReceive);
      if(lilEngineThatCAN.getCanId() == EVDC::Message) {
        launchState = EVDC::getLaunchedState(msgReceive);
        
      }
      if(launchState == 42) { // if the evdc is launched
        progressBlock = false;
      }
      
    }
    else{
      delay(1);
    }
  
    if(millis() > progressionTimer) {
      RPi::giveProgression(lilEngineThatCAN,4); 
      EVDC::goForLaunch(lilEngineThatCAN);
      progressionTimer = millis() + 100;
    }
  }
  
  readyToDerpOn();
  
  RTDEndTime = millis() + 2500;
   
  
  return true;
}













































void startupDebug( MCP_CAN& lilEngineThatCAN) { //  safety checks? Pshhh
  RPi::giveProgression(lilEngineThatCAN,4);
  Serial.println("starting up DEBUG");
  Serial.println("starting up DEBUG");
  Serial.println("starting up DEBUG");
  
  while(getMultiplexerAnalog(FIVE_SUPPLY_CHECK_SELECT) < 500) { // while the LV system has not been turned on
    EVDC::calmTheFreakDown(lilEngineThatCAN, 64);
    RPi::giveProgression(lilEngineThatCAN, 4);
    Serial.println("LV system not powered");
    delay(100);
  }

  
  digitalWrite(AIRdcdc, HIGH);  
  digitalWrite(software_pushbutton_control, HIGH);
  digitalWrite(software_shutdown_control, HIGH);
  readyToDerpOn();
  startLightOn();
  for(int i = 0; i < 35; i++) {
    RPi::giveProgression(lilEngineThatCAN,4);
    if(i > 30) {
      EVDC::goForLaunch(lilEngineThatCAN);
    }
    else{
      EVDC::calmTheFreakDown(lilEngineThatCAN, 64);
    }
    delay(100);
    Serial.println("precharging");
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
  pinMode(newBoostButtonPin, INPUT);
  pinMode(IMDpin, INPUT);
  pinMode(twenty_four_thermistor, INPUT);
  pinMode(MULTIPLEXER_SELECT_3, OUTPUT);
  pinMode(MULTIPLEXER_SELECT_2, OUTPUT);
  pinMode(MULTIPLEXER_SELECT_1, OUTPUT);

  
}
