#ifndef CAR_FUNCS
#define CAR_FUNCS

#include <stddef.h> 
#include "pinAndErrorDefinitions.h"


/*
list of functions:
getMultiplexerDigital
getMultiplexerAnalog
BMSlightOn
BMSlightOff
IMDlightOn
IMDlightOff
startLightOn
startLightOff
shutdownButton
boostButton
cycleButton
toggleButton
memcpyInt
memcpyLong
testMicroBoard
*/


boolean getMultiplexerDigital(char selection) {
  digitalWrite(MULTIPLEXER_SELECT_0, LOW);
  digitalWrite(MULTIPLEXER_SELECT_1, LOW);
  digitalWrite(MULTIPLEXER_SELECT_2, LOW);
  digitalWrite(MULTIPLEXER_SELECT_3, LOW);
  
  if((selection & 0x01) == 0x01) {
    digitalWrite(MULTIPLEXER_SELECT_0, HIGH);
  }
  if((selection & 0x02) == 0x02) {
    digitalWrite(MULTIPLEXER_SELECT_1, HIGH);
  }
  if((selection & 0x04) == 0x04) {
    digitalWrite(MULTIPLEXER_SELECT_2, HIGH);
  }
  if((selection & 0x08) == 0x08) {
    digitalWrite(MULTIPLEXER_SELECT_3, HIGH);
  }
  delayMicroseconds(1) ; //for propagation delay
  return digitalRead(MULTIPLEXER_INPUT);
}



int getMultiplexerAnalog(char selection) {
  digitalWrite(MULTIPLEXER_SELECT_0, LOW);
  digitalWrite(MULTIPLEXER_SELECT_1, LOW);
  digitalWrite(MULTIPLEXER_SELECT_2, LOW);
  digitalWrite(MULTIPLEXER_SELECT_3, LOW);
  
  if((selection & 0x01) == 0x01) {
    digitalWrite(MULTIPLEXER_SELECT_0, HIGH);
  }
  if((selection & 0x02) == 0x02) {
    digitalWrite(MULTIPLEXER_SELECT_1, HIGH);
  }
  if((selection & 0x04) == 0x04) {
    digitalWrite(MULTIPLEXER_SELECT_2, HIGH);
  }
  if((selection & 0x08) == 0x08) {
    digitalWrite(MULTIPLEXER_SELECT_3, HIGH);
  }
  delayMicroseconds(1) ; //for propagation delay
  return analogRead(MULTIPLEXER_INPUT);
}

void BMSlightOn() {
  digitalWrite(BMS_LED, HIGH);
}

void BMSlightOff() {
  digitalWrite(BMS_LED, LOW);
}

void IMDlightOn() {
  digitalWrite(IMD_LED, HIGH);
}

void IMDlightOff() {
  digitalWrite(IMD_LED, LOW);
}

void startLightOn() {
  digitalWrite(start_LED, HIGH);
}

void startLightOff() {
  digitalWrite(start_LED, LOW);
}

boolean shutdownButton() {
  return getMultiplexerDigital(SHUTDOWN_BUTTON_SELECT);
}

boolean boostButton() {
  return getMultiplexerDigital(BOOST_BUTTON_SELECT);
}

boolean cycleButton() {
  return getMultiplexerDigital(CYCLE_BUTTON_SELECT);
}

boolean toggleButton() {
  return getMultiplexerDigital(TOGGLE_BUTTON_SELECT);
}




void testMicroBoard(MCP_CAN& greenBus) {
   while(CAN_OK != greenBus.begin(CAN_500KBPS)) { // CAN_OK != greenBus.begin(CAN_500KBPS)
     
  Serial.println("can bus is not operational");
    
    delay(10);
  }
  
  pinMode(10, INPUT);
  delay(100);
  Serial.println("Can Bus init success");
  boolean digitalTest = true;
  while(1) {
    Serial.println("digital test");
    digitalWrite(readyToDriveSound, digitalTest);
    digitalWrite(AirDCDC, digitalTest);
    digitalWrite(software_shutdown_control, digitalTest);
    digitalWrite(software_pushbutton_control, digitalTest);
    digitalWrite(start_LED, digitalTest);
    digitalWrite(fan_control, digitalTest);
    digitalWrite(pump_control, digitalTest);
    digitalWrite(IMD_LED, digitalTest);
    digitalWrite(BMS_LED, digitalTest);
    digitalWrite(CONTROL_11, digitalTest);
    digitalWrite(CONTROL_10, digitalTest);
    Serial.print("IMD pin: ");
    if(digitalRead(IMDpin)) {
      Serial.println(analogRead(IMDpin)); //high
    }
    else {
      Serial.println(analogRead(IMDpin));
    }
    
    Serial.println("analog test");
    Serial.print("five volt check: ");
    Serial.println(analogRead(twenty_four_thermistor));
    Serial.println("multiplexer test");
    for(int i = 0; i < 16; i++) {
      Serial.print(i);
      Serial.write(32);
      Serial.println(getMultiplexerAnalog(char(i)));
    }
    digitalWrite(MULTIPLEXER_SELECT_0, digitalTest);
    digitalWrite(MULTIPLEXER_SELECT_1, digitalTest);
    digitalWrite(MULTIPLEXER_SELECT_2, digitalTest);
    digitalWrite(MULTIPLEXER_SELECT_3, digitalTest);
    delay(1000);
    digitalTest = !digitalTest;
  }
}

#endif

