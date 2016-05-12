
#include <SPI.h>
#include <mcp_can.h>
#include <mcp_can_dfs.h>

MCP_CAN MCcan(9); // SEED shiled goes to MC
MCP_CAN CarCAN(10); // sparkfun shield goes to rest of car

long messageA0counter = 0;
long messageA1counter = 0;
long messageA2counter = 0;
long messageA3counter = 0;
long messageA4counter = 0;
long messageA5counter = 0;
long messageA6counter = 0;
long messageA7counter = 0;
long messageAAcounter = 0;
long messageABcounter = 0;
long messageACcounter = 0;

long messageEVDCtimer = 0;
#define MOTOR_CONTR_MESSAGE_DELAY 20 // 50 control messages per second. lols.

boolean nextMessageDisable = false;
unsigned char disable[8] = {0,0,0,0,0,0,0,0};


long messageTimeoutCheck = 0;

long MCtimeout = 0;


unsigned char tempBuf[8];

unsigned char lastA0message[8];
unsigned char lastA1message[8];
unsigned char lastA2message[8];
unsigned char lastA3message[8];
unsigned char lastA4message[8];
unsigned char lastA5message[8];
unsigned char lastA6message[8];
unsigned char lastA7message[8];
unsigned char lastAAmessage[8];
unsigned char lastABmessage[8];
unsigned char lastACmessage[8];

unsigned char lastEVDCmessage[8] = {0,0,0,0,0,0,0,0};







void setup() {
  Serial.begin(9600);
  
  while(CAN_OK != MCcan.begin(CAN_500KBPS)) {
    Serial.println("MC bus not operational");
    delay(10);
  }
  while(CAN_OK != CarCAN.begin(CAN_500KBPS)){
    Serial.println("car bus not operational");
    delay(10);
  }
  
  CarCAN.init_Filt(0, 0, 0x0C0); // scan only for command messages
  
  Serial.println("all good");

}

void loop() {
  
  unsigned char len = 0;
  
  if(CAN_MSGAVAIL == MCcan.checkReceive()) {
    MCcan.readMsgBuf(&len, tempBuf);
    //Serial.println("M");
    switch(MCcan.getCanId()) {
      case 0x0A0:
        for(int i = 0; i < 8; i++) {
          lastA0message[i] = tempBuf[i];
        }  
        messageA0counter++;
        break;
      case 0x0A1:
        for(int i = 0; i < 8; i++) {
          lastA1message[i] = tempBuf[i];
        }
        messageA1counter++;
        break;
      case 0x0A2:
        for(int i = 0; i < 8; i++) {
          lastA2message[i] = tempBuf[i];
        }
        messageA2counter++;
        break;
      case 0x0A3:
        for(int i = 0; i < 8; i++) {
          lastA3message[i] = tempBuf[i];
        }
        messageA3counter++;
        break;
      case 0x0A4:
        for(int i = 0; i < 8; i++) {
          lastA4message[i] = tempBuf[i];
        }
        messageA4counter++;
        break;
      case 0x0A5:
        for(int i = 0; i < 8; i++) {
          lastA5message[i] = tempBuf[i];
        }
        messageA5counter++;
        break;
      case 0x0A6:
        for(int i = 0; i < 8; i++) {
          lastA6message[i] = tempBuf[i];
        }
        messageA6counter++;
        break;
      case 0x0A7:
        for(int i = 0; i < 8; i++) {
          lastA7message[i] = tempBuf[i];
        }
        messageA7counter++;
        break;
      case 0x0AA:
        for(int i = 0; i < 8; i++) {
          lastAAmessage[i] = tempBuf[i];
        }
        messageAAcounter++;
        break;
      case 0x0AB:
        for(int i = 0; i < 8; i++) {
          lastABmessage[i] = tempBuf[i];
        }
        messageABcounter++;
        break;
      case 0x0AC:
        for(int i = 0; i < 8; i++) {
          lastACmessage[i] = tempBuf[i];
        }
        messageACcounter++;
        break;
    }
  }
  
  if(messageA0counter > 1) {
    CarCAN.sendMsgBuf(0x0A0, 0, 8, lastA0message);
    messageA0counter = 0;
  }
  if(messageA1counter > 1) {
    CarCAN.sendMsgBuf(0x0A1, 0, 8, lastA1message);
    messageA1counter = 0;
  }
  if(messageA2counter > 1) {
    CarCAN.sendMsgBuf(0x0A2, 0, 8, lastA2message);
    messageA2counter = 0;
  }
  if(messageA3counter > 7) {
    CarCAN.sendMsgBuf(0x0A3, 0, 8, lastA3message);
    messageA3counter = 0;
  }
  if(messageA4counter > 7) {
    CarCAN.sendMsgBuf(0x0A4, 0, 8, lastA4message);
    messageA4counter = 0;
  }
  if(messageA5counter > 7) {
    CarCAN.sendMsgBuf(0x0A5, 0, 8, lastA5message);
    messageA5counter = 0;
  }
  if(messageA6counter > 7) {
    CarCAN.sendMsgBuf(0x0A6, 0, 8, lastA6message);
    messageA6counter = 0;
  }
  if(messageA7counter > 7) {
    CarCAN.sendMsgBuf(0x0A7, 0, 8, lastA7message);
    messageA7counter = 0;
  }
  if(messageAAcounter > 1) {
    CarCAN.sendMsgBuf(0x0AA, 0, 8, lastAAmessage);
    messageAAcounter = 0;
  }
  if(messageABcounter > 1) {
    CarCAN.sendMsgBuf(0x0AB, 0, 8, lastABmessage);
    messageABcounter = 0;
  }
  if(messageACcounter > 7) {
    CarCAN.sendMsgBuf(0x0AC, 0, 8, lastACmessage);
    messageACcounter = 0;
  }
  
  if(CAN_MSGAVAIL == CarCAN.checkReceive()) {
    //Serial.println("C");
    CarCAN.readMsgBuf(&len, tempBuf);
    if(CarCAN.getCanId() == 0x0C0) {
    
        for(int i = 0; i < 8; i++) {
          lastEVDCmessage[i] = tempBuf[i];
        }
      
      
      
      messageTimeoutCheck = millis() + 2000;
      
    }
  }
  
  if(millis() > messageTimeoutCheck) {
    for(int i = 0; i < 8; i++) {
      lastEVDCmessage[i] = 0;
    }
  }
  
  if(millis() > messageEVDCtimer){
    
      MCcan.sendMsgBuf(0x0C0, 0, 8, lastEVDCmessage);
    
    messageEVDCtimer = millis() + MOTOR_CONTR_MESSAGE_DELAY;
  }
  
  
}
