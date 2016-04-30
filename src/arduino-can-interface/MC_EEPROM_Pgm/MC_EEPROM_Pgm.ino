#include <mcp_can.h>
#include <math.h>
#include <SPI.h>
#include <string.h>

// the cs pin of the version after v1.1 is default to D9
// v0.9b and v1.0 is default D10
const int SPI_CS_PIN = 10;

MCP_CAN CAN(SPI_CS_PIN); 

void setup() {
  Serial.begin(9600);

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
}

unsigned char msg[8] = {142, 0, 1, 0, 0, 1, 0, 0, 0};

void loop() {
  Serial.println("Hit a to program");
  char entry = Serial.read();
  if (entry == 'a') {
    CAN.sendMsgBuf(0x0C1, 0, 8, msg);
  }

   if(CAN_MSGAVAIL == CAN.checkReceive()) {
    unsigned char[8] buf;
    CAN.readMsgBuf(8, buf);
    if (CAN.getCanId() == 0x0C2) {
      Serial.print(buf[2]);
    }
   }
}
