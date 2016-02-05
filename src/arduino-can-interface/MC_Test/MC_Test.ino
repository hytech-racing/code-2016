/**
 * Victor Chen
 * 19 Jan 2016
 * Sends mock motor controller data over CAN 
 */
#include <mcp_can.h>
#include <math.h>
#include <SPI.h>

// the cs pin of the version after v1.1 is default to D9
// v0.9b and v1.0 is default D10
const int SPI_CS_PIN = 9;

MCP_CAN CAN(SPI_CS_PIN);                                    // Set CS pin

void setup()
{
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
int timer0 = 0;
//int timer1 = (timer0-(timer3*pow(2.0, 24.0) + timer2*pow(2.0, 16.0))/pow(2.0, 8.0));
//int timer2 = (timer0 % pow(2.0, 4.0))/pow(2.0, 16.0);
//int timer3 = timer0/pow(2.0, 4.0);
unsigned char msg1[8] = {3, 232, 3, 200, 3, 195, 1, 244};
unsigned char msg2[8] = {1, 144, 2, 88, 2, 88, 0, 0};
unsigned char msg3[8] = {0, 0, 0, 0, 3, 132, 0, 0};
unsigned char msg4[8] = {1, 0, 0, 0, 0, 0, 0, 0};
unsigned char msg5[8] = {0, 0, 19, 136, 0, 0, 0, 0};
unsigned char msg6[8] = {3, 32, 3, 32, 3, 32, 3, 32};
unsigned char msg7[8] = {7, 208, 7, 208, 7, 208, 7, 208};
unsigned char msg8[8] = {0, 0, 4, 0, 0, 0, 0b10000001, 0 };
unsigned char msg9[8] = {0, 0, 0, 0, 0, 0, 0, 0 };
unsigned char msg10[8] = {0, 0, 0, 0, 0, 0, 0, timer0};
void loop()
{
    // send data:  id = 0x00, standrad frame, data len = 8, stmp: data buf
    CAN.sendMsgBuf(0x0A0, 0, 8, msg1);
    CAN.sendMsgBuf(0x0A1, 0, 8, msg2);
    CAN.sendMsgBuf(0x0A2, 0, 8, msg3);
    CAN.sendMsgBuf(0x0A4, 0, 8, msg4);
    CAN.sendMsgBuf(0x0A5, 0, 8, msg5);
    CAN.sendMsgBuf(0x0A6, 0, 8, msg6);
    CAN.sendMsgBuf(0x0A7, 0, 8, msg7);
    CAN.sendMsgBuf(0x0AA, 0, 8, msg8);
    CAN.sendMsgBuf(0x0AB, 0, 8, msg9);
    CAN.sendMsgBuf(0x0AC, 0, 8, msg10);
    timer0++;
    delay(100);                       // send data per 100ms
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
