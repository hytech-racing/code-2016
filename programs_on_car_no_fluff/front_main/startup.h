//#include <SPI.h>
//#include "mcp_can.h"
//#include "mc.h"
//#include "evdc.h"
//#include "bms.h"
//#include "ar.h"
//#include "pi.h"
//#include "IMD.h"
/*


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
  int lightState;
  long progressionTimer = millis();
  
  
  #define readyToDerpSound 0
#define AirDCDC  1
#define AIRdcdc  1
#define software_shutdown_control  2 //OKAY
#define software_pushbutton_control 3 

  digitalWrite(AIRdcdc, LOW);
  digitalWrite(software_shutdown_control, LOW);
  digitalWrite(software_pushbutton_control, LOW);
  
  startLightOn();
  Serial.println("start light on top of sequence");
  delay(1000);


  // I want a delay for all the other systems to boot up.
  for(int i = 0; i < 8; i++) {  
    startLightOn();
    delay(100);
    startLightOff();
    delay(100); 
    RPi::giveProgression(lilEngineThatCAN,0);
    EVDC::calmTheFreakDown(lilEngineThatCAN, 65);
    delay(100); 

  }
    

  
 // here I wait for OKHS to go high. if it does not go high in 5 seconds, then it decides that the IMD is bad and throws an error
 startLightOff();
  progressBlock = true;
  int counter = 0;

  Serial.println("IMD check");
  while(analogRead(IMDpin) < 300) {
    delay(100);
    EVDC::calmTheFreakDown(lilEngineThatCAN, 73);
    RPi::giveProgression(lilEngineThatCAN,0); 

    Serial.print("IMD: ");
    Serial.println(analogRead(IMDpin));
    counter++;
    if (counter > 50) { // 5 seconds for the IMD to turn on
      while(1) {
        IMDlightOn();
      }
    }
  }
  

// here I check for brakes and the button to be pressed in order to turn on the tractive system.
  
  int brakes = 0;
  
  while(progressBlock) {
    if(millis() > progressionTimer) {
      RPi::giveProgression(lilEngineThatCAN,3); 
      EVDC::calmTheFreakDown(lilEngineThatCAN, 71);
      lightState = 1-lightState;
      if(lightState > 0) {
        startLightOn();
      }
      else{
        startLightOff();
      }
      progressionTimer += 100;
    } 
    if(CAN_MSGAVAIL == lilEngineThatCAN.checkReceive()) {
      lilEngineThatCAN.readMsgBuf(&len, msgReceive);
      if(lilEngineThatCAN.getCanId() == EVDC::Message) {
        brakes = EVDC::getBrakes(msgReceive);
        Serial.println("brakes ");
        Serial.println(brakes);
      }
    }
    if((getMultiplexerAnalog(IMD_PWM_HIGH_SELECT) < 500) && (brakes > 25)) { // if the progress button is pressed and brakes are more than 25%
      progressBlock = false;
      Serial.println("both conditions met");
    }
  }
  
  
  //here I turn on the high voltage system and wait for the DC bus voltage to be higher than
  
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
  delay(1500); // 1.5 seconds to precharge
  digitalWrite(software_pushbutton_control, LOW);
  progressBlock = true;
  
  // now I wait for the button to go low so that you can't just hold down the button in order to progress- they have to let go of the button first
  
  while(getMultiplexerAnalog(IMD_PWM_HIGH_SELECT) < 500) {
    delay(10);
  }
  delay(50); // for debouncing
  
  // now I wait for the progress button in order to send the ready-to-drive signal
    
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
    if(getMultiplexerAnalog(IMD_PWM_HIGH_SELECT) < 500) {
      progressBlock = false;
    }
  }
  startLightOff();
 

  // now I send the ready-to-go signal to the EVDC and wait for its response from the motor controller

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
    
    if(millis() > progressionTimer) {
      RPi::giveProgression(lilEngineThatCAN,4); 
      EVDC::goForLaunch(lilEngineThatCAN);
      progressionTimer = millis() + 100;
    }
  }
  
  // turn on ready to drive sound and set the sound's end timer
  
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
