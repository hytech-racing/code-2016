/**
 * Ryan Gallaway
 * Sep 1 2015
 * If 'r' is input, sends msg on ID 0x01
 * If 's' is input, sends msg on ID 0x02
 * Listens for reply on IDs 0x14 and 0x15
 */


#include <SPI.h>
#include "mcp_can.h"

const int SPI_CS_PIN = 9;
MCP_CAN CAN(SPI_CS_PIN);
int sent = 0;

void setup() {
  Serial.begin(9600);

  START_INIT:
  if(CAN_OK == CAN.begin(CAN_500KBPS)) {
    Serial.println("CAN BUS Shield Init OK");
  }
  else {
    Serial.println("CAN BUS Shield init FAIL");
    Serial.println("Init CAN BUS Shield again");
    delay(100);
    goto START_INIT;
  }
}

void loop() {
  unsigned char len = 0;
  unsigned char buf[8];
  unsigned char msg[8] = {0,0,0,0,0,0,0,0};

  if(!sent) {
    char entry;
    entry = Serial.read();
    if(entry == 'r') {
      CAN.sendMsgBuf(0x01, 0, 8, msg);
      Serial.println("sent request 0x01");
      sent = 1;
    }
    else if(entry == 's') {
      CAN.sendMsgBuf(0x02, 0, 8, msg);
      Serial.println("sent request 0x02");
      sent = 1;
    }
  }

  if(CAN_MSGAVAIL == CAN.checkReceive()) {
    CAN.readMsgBuf(&len, buf);
    if(CAN.getCanId() == 0x15) {
      Serial.print("Potentiometer value: ");
      Serial.println(buf[0]);
      sent = 0;
    }
    else if(CAN.getCanId() == 0x14) {
      Serial.print("Received value: ");
      Serial.println(buf[0]);
      sent = 0;
    }
  }
}

/*String input(String message) {    // Serial input function routine. Declare inData global String, usage input("message string");
  String inData;                  // buffer contain data from serial input function
  char received;                  // Each character received
  inData = "";                    // Clear recieved buffer  
  Serial.print(message);

  while (received != '\n') {      // When new line character is received (\n = LF, \r = CR)
    if (Serial.available() > 0)   // When character in serial buffer read it
    {
      received = Serial.read();  
      Serial.print(received);     // Echo each received character, terminal dont need local echo
      inData += received;         // Add received character to inData
    }
  }
  inData.trim();                  // Eliminate \n, \r, blank and other not "printable"
  return inData;                  // Return the string character(s)
}*/
