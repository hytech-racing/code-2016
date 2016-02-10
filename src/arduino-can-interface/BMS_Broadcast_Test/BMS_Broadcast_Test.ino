/**
 * Ryan Gallaway
 * 27 Oct 2015
 * Sends data over CAN Bus formatted the same way
 * as the Orion BMS
 */

#include <SPI.h>
#include "mcp_can.h"

const int SPI_CS_PIN = 10; //9 if using SEEEDStudio shield
MCP_CAN CAN(SPI_CS_PIN);
int sent = 0;

void setup() {
  Serial.begin(9600); //initialize serial for PC communication

  /*
    Initializes CAN controller
  */
  START_INIT:
  if(CAN_OK == CAN.begin(CAN_500KBPS)) { //Communication rate is 500Kbps, should be fine for normal use
    Serial.println("CAN BUS Shield Init OK");
  }
  else {
    Serial.println("CAN BUS Shield init FAIL");
    Serial.println("Init CAN BUS Shield again");
    delay(100);
    goto START_INIT; //Yes, I know it's a goto, I could rewrite it but I'm lazy
  }
}

void loop() {
  unsigned char msg1[8] = {0b10010100,194,99,0,20,0,119};
  unsigned char msg2[8] = {0,100,1,244,1,144,0,100};
  unsigned char msg3[8] = {8,152,8,152,8,152,0,1};
  unsigned char msg4[4] = {50,30,40,30};
  unsigned char msg5[6] = {13,172,120,128,13,22};
  unsigned char msg6[6] = {13,172,120,128,13,22};

  CAN.sendMsgBuf(0x001, 0, 8, msg1);
  delay(1);
  CAN.sendMsgBuf(0x002, 0, 8, msg2);
  delay(1);
  CAN.sendMsgBuf(0x003, 0, 8, msg3);
  delay(1);
  CAN.sendMsgBuf(0x004, 0, 4, msg4);
  delay(1);
  CAN.sendMsgBuf(0x005, 0, 6, msg5);
  delay(1);
  CAN.sendMsgBuf(0x006, 0, 6, msg6);
  delay(1);
  Serial.println("Sent messages");
  delay(90);
}
