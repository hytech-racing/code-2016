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
unsigned char bat[8] = {0,0,0,0,0,0,0,0};
unsigned char temp[8] = {70,0,65,0,0,0,0,0}; 
unsigned char mTemp[8] = {0,0,0,0,0,0,0,0};
unsigned char sseq[8] = {0,0,0,0,0,0,0,0};
unsigned char ah[8] = {0,0,0,0,0,0,200,0};
unsigned char draw[8] = {0,0,0,0,0,0,0,0};
unsigned char lv[8] = {0,0,20,0,0,0,0,0};
unsigned char pedals[8] = {0,42,0,42,0,0,0,0};
short torque = 0;
int enabled = 0;
bool up = true;
int pot = 1;
int state = 0;
int value;
int charge = 0;
int error = 0;

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
    if (up) {
      charge += 5;
      if (charge >= 200) {
        up = false;
      }
    } else {
      charge -= 5;
      if (charge <= 0) {
        up = true;
      }
    }
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
    } else if (entry == 'o') {
      state--;
      if (state == 8) {
        state = 5;
      } else if (state <= 0) {
        state = 0;
      }
    } else if (entry == '1') {
      error = 4;
    } else if (entry == '2') {
      error = 20;
    } else if (entry == '0') {
      error = 0;
    }
    int avg = random(1, 100);
    int high = random(avg, 100);
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
    int motor = 800;
    short current = -100;
    int thr = charge / 2;

      /**
       * INPUT SEQUENCE TO TURN ON MOTOR 100% OF THE TIME
       * Start sending disable messages
       * Then send real messages (press s)
       * Then send quick disable for lockout (press e)
       */
    memcpy(&msg[2], &torque, sizeof(short)); //speed mode (ignore the fact that it's called "torque")
    memcpy(&sseq[0], &state, sizeof(byte));
    memcpy(&mTemp[4], &motor, sizeof(short));
    memcpy(&draw[6], &current, sizeof(short));
    memcpy(&bat[1], &charge, sizeof(byte));
    memcpy(&pedals[3], &thr, sizeof(byte));
    memcpy(&sseq[1], &error, sizeof(byte));

    Serial.print("Startup: ");
    Serial.print(sseq[0]);
    Serial.print("  Error: ");
    Serial.println(sseq[1]);
    CAN.sendMsgBuf(0x020, 0, 8, lv);
    delay(5);
    CAN.sendMsgBuf(0x0C8, 0, 8, pedals);
    delay(5);
    CAN.sendMsgBuf(0x0A2, 0, 8, mTemp);
    delay(5);
    CAN.sendMsgBuf(0x0A5, 0, 8, msg);
    delay(5);
    CAN.sendMsgBuf(0x001, 0, 8, bat);
    delay(5);
    CAN.sendMsgBuf(0x004, 0, 8, temp);
    delay(5);
    CAN.sendMsgBuf(0x010, 0, 8, sseq);
    delay(5);
    CAN.sendMsgBuf(0x0A6, 0, 8, draw);
    delay(5);
  }

