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
unsigned char bat[8] = {0,150,0,0,0,0,0,0};
unsigned char temp[8] = {70,0,65,0,0,0,0,0}; 
unsigned char sseq[8] = {0,0,0,0,0,0,0,0};
byte torque = 0;
int enabled = 0;
int bPercent = 0;
bool up = true;
int pot = 1;
int state = 0;
int value;

void loop() {
//  if (!enabled) {
//    CAN.sendMsgBuf(0x0C0, 0, 8, lockout);
//    Serial.println("inverter disable");
//    char entry;
//    entry = Serial.read();
//    if (entry == 's') {
//      CAN.sendMsgBuf(0x0C0, 0, 8, enable);
//      torque = 0;
//      Serial.println("enabled");
//      enabled = 1;
//    }
//    delay(50);
//  } else {
    digitalWrite(A3, HIGH);
    digitalWrite(A5, LOW);
    if (pot == 1) {
      value = analogRead(A4);
      if (value < 50) {
        torque = 0;
      } else {
        torque = value;
      }
      pot = 1;
    }
    char entry = Serial.read();
    if (entry == 'p') {
      state++;
      if (state > 5) {
        state = 8;
      }
    }
//    if (Serial.available() > 0) {
//      char entry;
//      entry = Serial.read();
//      if (entry == 'd') {
//        CAN.sendMsgBuf(0x0C0, 0, 8, enable);
//        Serial.println("disable");
//        enabled = 0;
//        pot = 0;
//      } else if (entry == 'e') {
//        CAN.sendMsgBuf(0x0C0, 0, 8, lockout);
//      } else if (entry == 'a') {
//        torque = 120;
//        pot = 0;
//      } else if (entry == 'r') {
//        torque = -100;
//        pot = 0;
//      } else if (entry == 'f') {
//        torque = 0;
//        pot = 0;
//      }

      /**
       * INPUT SEQUENCE TO TURN ON MOTOR 100% OF THE TIME
       * Start sending disable messages
       * Then send real messages (press s)
       * Then send quick disable for lockout (press e)
       */
    memcpy(&msg[2], &torque, sizeof(short)); //speed mode (ignore the fact that it's called "torque")
    memcpy(&bat[1], &bPercent, sizeof(short));
    Serial.println(state);
    CAN.sendMsgBuf(0x0A5, 0, 8, msg);
    CAN.sendMsgBuf(0x001, 0, 8, bat);
    CAN.sendMsgBuf(0x004, 0, 8, temp);
    CAN.sendMsgBuf(0x010, 0, 8, sseq);
    delay(20);
  }

