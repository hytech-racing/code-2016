/**
 * Ryan Gallaway
 * 27 Oct 2015
 * Receives CAN data and parses using libraries
 */

#include <SPI.h>
#include "mcp_can.h"
#include "bms.h"

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
  unsigned char len = 0; //Length of received message
  unsigned char buf[8];  //Where the received message will be stored
  if (CAN_MSGAVAIL == CAN.checkReceive()) { //Checks if message is available
    CAN.readMsgBuf(&len, buf); //If so, stores length & value

    // You have to readMsgBuf before you can get the ID of the message

    if (CAN.getCanId() == 0x01) { //Filters so only certain messages will execute this code
      Serial.print("Current supply V: ");
      Serial.println(BMS::getSupplyVoltage(buf));
    } else if (CAN.getCanId() == 0x60) {
      Serial.print(buf[0]);
      Serial.print(" ");
      Serial.print(buf[1]);
      Serial.print(" ");
      Serial.print(buf[2]);
      Serial.print(" ");
      Serial.print(buf[3]);
      Serial.print(" ");
      Serial.print(buf[4]);
      Serial.print(" ");
      Serial.print(buf[5]);
      Serial.print(" ");
      Serial.print(buf[6]);
      Serial.print(" ");
      Serial.println(buf[7]);
    }
  }
}

