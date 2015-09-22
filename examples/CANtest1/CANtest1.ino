// Sanjay Sood
// 9/1/15 CAN test code
// This module was written a network
// test of three arduino's linked together
// The arduino waits for a request and sends
// a meaningless response to verify connecectivity 

#include <mcp_can.h>
#include <SPI.h>

// Sparkfun Shield uses pin #10 
int SPI_CS_PIN = 10;

//Set up CS pin
MCP_CAN CAN(SPI_CS_PIN);                                

void setup()
{
    Serial.begin(9600);

START_INIT:

    // init can bus : baudrate = 500k
    if(CAN_OK == CAN.begin(CAN_500KBPS))                   
    {
        Serial.println("CAN BUS Shield init ok!" );
    }
    else
    {
        Serial.println("CAN BUS Shield init fail");
        Serial.println("Init CAN BUS Shield again");
        delay(100);
        goto START_INIT;
    }
}

void loop()
{
    unsigned char len = 0;
    unsigned char buf[8];
    if (CAN_MSGAVAIL == CAN.checkReceive()) {
      CAN.readMsgBuf(&len,buf);
      // Listesn for 0x02 and returns a reply on 0x014 
      if (CAN.getCanId() == 0x02) {
        Serial.println("Request Received");
        unsigned char data[8];
        data[0] = 255;
        CAN.sendMsgBuf(0x14, 0, 8, data);
      }
  }
      
    delay(100);                       
}
