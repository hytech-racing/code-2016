#include <mcp_can.h>
#include <math.h>
#include <SPI.h>
#include <string.h>

//Decongester Board uses pin 9 for MC CAN Bus
const int SPI_CS_PIN = 9;

boolean stringComplete = false;
String inputString = "";
unsigned char msg[8] = {0, 0, 0, 0, 0, 0, 0, 0};

MCP_CAN CAN(SPI_CS_PIN); 

void setup() {
  Serial.begin(9600);
  inputString.reserve(200);

  START_INIT:

    if(CAN_OK == CAN.begin(CAN_500KBPS))                   // init can bus : baudrate = 500k
    {
        Serial.println("CAN BUS Shield init ok!");
    }
    else
    {
        Serial.println("CAN BUS Shield init fail");
        Serial.println("Init CAN BUS Shield again");
        delay(100);
        goto START_INIT;
    }

  Serial.println("Type an ID to read");
}



void loop() {
  if (stringComplete) {
    int entry = inputString.toInt();
    Serial.print("Checking ID ");
    Serial.println(entry);
    msg[0] = (unsigned char) entry;
    CAN.sendMsgBuf(0x0C1, 0, 8, msg);
    // clear the string:
    inputString = "";
    stringComplete = false;
    
  }
  
  if(CAN_MSGAVAIL == CAN.checkReceive()) {
    unsigned char buf[8];
    unsigned char len = 8;
    CAN.readMsgBuf(&len, buf);

    if (CAN.getCanId() == 0x0C2) {
      Serial.print(buf[2]);
      Serial.print(": ");
      Serial.print(buf[5], HEX);
      Serial.print(" ");
      Serial.println(buf[4], HEX);
    }
   }
}

void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if (inChar == '\n') {
      stringComplete = true;
    } else {
      inputString += inChar;
    }
  }
}
