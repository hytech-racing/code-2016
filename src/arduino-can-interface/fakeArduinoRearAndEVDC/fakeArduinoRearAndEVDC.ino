/**
 * Ryan Gallaway and Andrew Renuart
 * 6 Feb 2015
 * Sends data over CAN Bus formatted the same way
 * as the rear arduino
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
  unsigned char msg1[8] = {0,35,27,188,89,42,0};
  unsigned char msg2[8] = {0,35,27,188,0,0,0};


  CAN.sendMsgBuf(0x2A2, 0, 8, msg1);
  delay(1);
  CAN.sendMsgBuf(0x0DC, 0, 8, msg2);
  
  

  Serial.println("Sent messages");
  delay(97);
  delayMicroseconds(476); // so that they don't synchronize
}
