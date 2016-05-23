#include <mcp_can.h>
#include <math.h>
#include <SPI.h>
#include <string.h>

//Decongester Board uses pin 9 for MC CAN Bus
const int SPI_CS_PIN = 9;

boolean stringComplete = false;
String inputString = "";
unsigned char msg[8] = {0, 0, 0, 0, 0, 0, 0, 0};
int id = 100;
boolean done = false;
long timer;

MCP_CAN CAN(SPI_CS_PIN);

void setup() {
  Serial.begin(9600);
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
  Serial.println("Waiting 10 seconds...");
  delay(10000);
  Serial.println("Beginning");
  timer = millis();
}

void loop() {
  if (millis() > timer) {
    timer = millis() + 500;
    if (id < 500) {
      Serial.print("Checking ID ");
      Serial.println(id);
      msg[0] = (unsigned char) id;
      CAN.sendMsgBuf(0x0C1, 0, 8, msg);
      id++;
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
      Serial.print(buf[2]);
      Serial.print(": ");
      Serial.print(buf[5], HEX);
      Serial.print(" ");
      Serial.println(buf[4], HEX);
    }
   }
}
