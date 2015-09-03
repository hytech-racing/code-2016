/**
 * Ryan Gallaway
 * Sep 1 2015
 * If 'r' is input, sends msg on ID 0x01
 * If 's' is input, sends msg on ID 0x02
 * Listens for reply on IDs 0x14 and 0x15
 */


#include <SPI.h>
#include "mcp_can.h"

/*
  Sets the pin Arduino uses for the CAN controller.
  Set to 9 if using a SEEEDStudio (black) shield
  Set to 10 if using a SparkFun (red) shield
*/
const int SPI_CS_PIN = 9;
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
  unsigned char msg[8] = {0,0,0,0,0,0,0,0}; //Message to be sent

  if(!sent) {
    char entry;
    entry = Serial.read();  //Reading from serial input
    if(entry == 'r') {
      /*
        FORMAT OF A CAN MESSAGE
        CAN.sendMsgBuf(ID, type, length, message)
        ID: from 0x00 to 0x7FF, identifies & prioritizes the message being sent
        type: 0 or 1, 0 for standard frame, 1 for extended (we're using standard)
        length: 0 to 8 (bytes)
        message: the message to be sent (max 8 bytes)
      */
      CAN.sendMsgBuf(0x01, 0, 8, msg); //This is the line that actually sends the CAN message
      Serial.println("sent request 0x01");
      sent = 1;
    }
    else if(entry == 's') {
      CAN.sendMsgBuf(0x02, 0, 8, msg); //Different message being sent, so different ID
      Serial.println("sent request 0x02");
      sent = 1;
    }
  }

  /*
    This section checks if a message is currently circulating on the bus.
  */
  if(CAN_MSGAVAIL == CAN.checkReceive()) { //Checks if message is available
    CAN.readMsgBuf(&len, buf); //If so, stores length & value

    // You have to readMsgBuf before you can get the ID of the message

    if(CAN.getCanId() == 0x15) { //Filters so only certain messages will execute this code
      Serial.print("Potentiometer value: ");
      Serial.println(buf[0]);
      sent = 0;
    }
    else if(CAN.getCanId() == 0x14) { //Same thing, different filter
      Serial.print("Received value: ");
      Serial.println(buf[0]);
      sent = 0;
    }
  }
}
