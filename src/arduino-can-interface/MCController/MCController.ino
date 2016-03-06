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
    pinMode(A1, OUTPUT);
    pinMode(A5, OUTPUT);
    pinMode(A3, INPUT);

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
short torque = 0;
int enabled = 0;
int pot = 0;
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
//    digitalWrite(A1, HIGH);
//    digitalWrite(A5, LOW);
//    if (pot == 1) {
//      value = analogRead(A3);
//      if (value < 50) {
//        torque = 0;
//      } else {
//        torque = value;
//      }
//      pot = 1;
//      Serial.print("Using pot ");
//    }
    if (Serial.available() > 0) {
      char entry;
      entry = Serial.read();
      if (entry == 'd') {
        CAN.sendMsgBuf(0x0C0, 0, 8, enable);
        Serial.println("disable");
        enabled = 0;
        pot = 0;
      } else if (entry == 'e') {
        CAN.sendMsgBuf(0x0C0, 0, 8, lockout);
      } else if (entry == 'a') {
        torque = 120;
        pot = 0;
      } else if (entry == 'r') {
        torque = -100;
        pot = 0;
      } else if (entry == 'f') {
        torque = 0;
        pot = 0;
      }

      /**
       * INPUT SEQUENCE TO TURN ON MOTOR 100% OF THE TIME
       * Start sending disable messages
       * Then send real messages (press s)
       * Then send quick disable for lockout (press e)
       */
    }
    memcpy(&msg[0], &torque, sizeof(short)); //speed mode (ignore the fact that it's called "torque")
    Serial.print(msg[0], HEX);
    Serial.print(" ");
    Serial.println(msg[1], HEX);
    CAN.sendMsgBuf(0x0C0, 0, 8, msg);
    delay(50);
  }
}

