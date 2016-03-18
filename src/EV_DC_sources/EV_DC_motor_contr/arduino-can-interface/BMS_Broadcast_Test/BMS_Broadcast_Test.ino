/**
 * Ryan Gallaway
 * 27 Oct 2015
 * Sends data over CAN Bus formatted the same way
 * as the Orion BMS
 */

#include <SPI.h>
#include "mcp_can.h"

const int SPI_CS_PIN = 9; //9 if using SEEEDStudio shield
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
  unsigned char msg1[8] = {0,150,5,5,7,7,245,122};
  unsigned char msg2[5] = {0,100,0,200,122};
  unsigned char msg3[7] = {2,2,0,0,50,45,92};

  CAN.sendMsgBuf(0x6B2, 0, 8, msg1);
  CAN.sendMsgBuf(0x03B, 0, 5, msg1);
  CAN.sendMsgBuf(0x3CB, 0, 7, msg1);
  delay(96);
}
