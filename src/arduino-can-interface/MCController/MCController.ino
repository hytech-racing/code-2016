/**
 * Ryan Gallaway
 * 16 Feb 2016
 * Motor Controller Controller Motor
 */

#include <mcp_can.h>
#include <math.h>
#include <SPI.h>
#include <string.h>

// the cs pin of the version after v1.1 is default to D9
// v0.9b and v1.0 is default D10
const int SPI_CS_PIN = 9;

MCP_CAN CAN(SPI_CS_PIN);                                    // Set CS pin

void setup()
{
    Serial.begin(9600);
    pinMode(A3, OUTPUT);
    pinMode(A5, OUTPUT);
    pinMode(A4, INPUT);

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

unsigned char msg[8] = {0,0,0,0,0,2,0,0};
int value;

void loop() {
  digitalWrite(A3, HIGH);
  digitalWrite(A5, LOW);
  value = analogRead(A4);
  memcpy(&msg[2], &value, sizeof(short));
  Serial.println(value);
  CAN.sendMsgBuf(0x0C0, 0, 8, msg);
  delay(50);
}

