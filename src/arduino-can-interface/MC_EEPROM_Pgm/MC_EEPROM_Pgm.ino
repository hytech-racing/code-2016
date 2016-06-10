#include <mcp_can.h>
#include <math.h>
#include <SPI.h>
#include <string.h>

// the cs pin of the version after v1.1 is default to D9
// v0.9b and v1.0 is default D10
// Pin 9 is used for MC_CAN on decongester
const int SPI_CS_PIN = 9;

MCP_CAN CAN(SPI_CS_PIN);

unsigned char len;

void setup() {
  Serial.begin(115200);

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
    Serial.println("Hit a to program");
}

unsigned char msg[8] = {0x94, 0, 1, 0, 0x64, 0x9C, 0, 0};

void loop() {
  
  char entry = Serial.read();
  if (entry == 'a') {
    CAN.sendMsgBuf(0x0C1, 0, 8, msg);
    Serial.println("Sent message");
  }

  if(CAN_MSGAVAIL == CAN.checkReceive()) {
    unsigned char buf[8];
    CAN.readMsgBuf(&len, buf);
    if (CAN.getCanId() == 0x0C2) {
      Serial.print(buf[2]);
    }
  }
}
