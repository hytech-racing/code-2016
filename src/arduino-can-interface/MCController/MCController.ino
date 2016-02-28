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

unsigned char lockout[8] = {0,0,0,0,0,0,0,0};
unsigned char enable[8] = {0,0,0,0,0,1,0,0};
unsigned char msg[8] = {0,0,0,0,1,1,0,0};
int torque = 0;
int enabled = 0;
int value;

void loop() {
  if (!enabled) {
    CAN.sendMsgBuf(0x0C0, 0, 8, lockout);
    Serial.println("inverter disable");
    char entry;
    entry = Serial.read();
    if (entry == 's') {
      CAN.sendMsgBuf(0x0C0, 0, 8, enable);
      torque = 0;
      Serial.println("enabled");
      enabled = 1;
    }
    delay(50);
  } else {
    if (Serial.available() > 0) {
      char entry;
      entry = Serial.read();
      if (entry == 'd') {
        CAN.sendMsgBuf(0x0C0, 0, 8, enable);
        Serial.println("disable");
        enabled = 0;
      } else if (entry == 'e') {
        CAN.sendMsgBuf(0x0C0, 0, 8, lockout);
      } else if (entry == 'a') {
  //    digitalWrite(A3, HIGH);
  //    digitalWrite(A5, LOW);
  //    value = analogRead(A4);
        torque = 40;
      } else if (entry == 'b') {
        torque = 120;
      } else if (entry == 'c') {
        torque = 200; 
      } else if (entry == 'f') {
        torque = 0;
      }

      /**
       * INPUT SEQUENCE TO TURN ON MOTOR 100% OF THE TIME
       * Start sending disable messages
       * Then send real messages (press s)
       * Then send quick disable for lockout (press e)
       */
    }
    memcpy(&msg[0], &torque, sizeof(short)); //torque mode
    Serial.print(msg[0], HEX);
    Serial.print(" ");
    Serial.println(msg[1]);
    CAN.sendMsgBuf(0x0C0, 0, 8, msg);
    delay(50);
  }
}

