#include <mcp_can.h>
#include <math.h>
#include <SPI.h>
#include <string.h>

//Decongester Board uses pin 9 for MC CAN Bus
const int SPI_CS_PIN = 9;

boolean stringComplete = false;
String inputString = "";
unsigned char msg[8] = {0, 0, 0, 0, 0, 0, 0, 0};
short id = 0;
boolean done = false;
long timer;

MCP_CAN CAN(SPI_CS_PIN);

void setup() {
  Serial.begin(115200);
  inputString.reserve(500);

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
  Serial.println("Waiting 5 seconds...");
  delay(5000);
  Serial.println("Beginning");
  timer = millis();
}

void loop() {
  if (millis() > timer) {
    timer = millis() + 100;
    if (id < 0x1F4) {
      /*Serial.print("Checking ID ");
      Serial.println(id);*/
      msg[0] = id & 0xFF;
      msg[1] = (id >> 8) & 0xFF;
      CAN.sendMsgBuf(0x0C1, 0, 8, msg);
      id++;
      Serial.print("C1: ");
      for(int i = 0; i < 8; i++) {
        Serial.print(msg[i], HEX);
        Serial.print(" ");
      }
      Serial.println("");
    } else if (!done) {
      Serial.println("######################");
      Serial.println("Done");
      Serial.println("######################");
      done = true;
    }
  }
  
  if(CAN_MSGAVAIL == CAN.checkReceive()) {
    unsigned char buf[8];
    unsigned char len = 8;
    CAN.readMsgBuf(&len, buf);

    if (CAN.getCanId() == 0x0C2) {
      Serial.print(0x0C2, HEX);
      Serial.print(": ");
      for(int i = 0; i < 8; i++) {
        Serial.print(buf[i], HEX);
        Serial.print(" ");
      }
      Serial.println("");
      Serial.println("");
    }
   }
}
