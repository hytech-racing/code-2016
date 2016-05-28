
#include <SPI.h>
#include <mcp_can.h>
#include <mcp_can_dfs.h>

MCP_CAN MCcan(9); // SEED shield goes to MC
MCP_CAN CarCAN(10); // Sparkfun shield goes to rest of car

int messageA0counter = 0;
int messageA1counter = 0;
int messageA2counter = 0;
int messageA3counter = 0;
int messageA4counter = 0;
int messageA5counter = 0;
int messageA6counter = 0;
int messageA7counter = 0;
int messageAAcounter = 0;
int messageABcounter = 0;
int messageACcounter = 0;

long messageEVDCtimer = 0;
#define MOTOR_CONTR_MESSAGE_DELAY 20 // 50 control messages per second. lols.

boolean nextMessageDisable = false;
unsigned char disable[8] = {0,0,0,0,0,0,0,0};
#define EVDC_TIMEOUT_LIMIT 500 // .5 seconds until an EVDC timeout is declared

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
  Serial.begin(115200);
  
  while(CAN_OK != MCcan.begin(CAN_500KBPS)) {
    Serial.println("MC bus not operational");
    delay(10);
  }
  while(CAN_OK != CarCAN.begin(CAN_500KBPS)){
    Serial.println("car bus not operational");
    delay(10);
  }
  
  CarCAN.init_Filt(0, 0, 0x0C0); // scan only for command messages
  
  Serial.println("MC BUS and Car Bus operational");

}

void loop() {
  
  unsigned char len = 0;
  
  if(CAN_MSGAVAIL == MCcan.checkReceive()) { // if a message is available from the MC, load it into 
                                             // a buffer and increment a counter
    MCcan.readMsgBuf(&len, tempBuf);
    //Serial.println("M");
    /*Serial.print(millis());
    Serial.print(" - ");
    Serial.print(MCcan.getCanId(), HEX);
    Serial.print(": ");
    for(int i = 0; i < 8; i++) {
      Serial.print(tempBuf[i], HEX);
      Serial.print(" ");
    }
    Serial.println("");*/
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
        Serial.print(0x0AA, HEX);
        Serial.print(": ");
        for(int i = 0; i < 8; i++) {
          Serial.print(tempBuf[i], HEX);
          Serial.print(" ");
        }
        Serial.println("");
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
      case 0x0AF:
        Serial.print(0x0AF, HEX);
        Serial.print(": ");
        for(int i = 0; i < 8; i++) {
          Serial.print(tempBuf[i], HEX);
          Serial.print(" ");
        }
        Serial.println("");
      default: // Otherwise rebroadcast message on Car CAN Bus
        CarCAN.sendMsgBuf(MCcan.getCanId(), 0, 8, tempBuf);
    }
  }
  
  // here, if the decongester has recieved a certain number of messages,
  // it will put a message out onto the Car CAN BUS
  
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
  
  // here, if a message is available on Car CAN bus,
  // it is rebroadcast on MC CAN bus
  
  if(CAN_MSGAVAIL == CarCAN.checkReceive()) {
    CarCAN.readMsgBuf(&len, tempBuf);
    MCcan.sendMsgBuf(CarCAN.getCanId(), 0, 8, tempBuf);
  }
  
}
