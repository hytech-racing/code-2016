/*
this program is to simulate the BMS, Motor Controller, and Rear board in order to test the morocontroller board
*/

#include <SPI.h>
#include "mcp_can.h"
const int SPI_CS_PIN = 9; //9 if using SEEEDStudio shield
MCP_CAN CAN(SPI_CS_PIN);



unsigned char MCmsg1[8] = {3, 232, 3, 200, 3, 195, 1, 244};
unsigned char MCmsg2[8] = {1, 144, 2, 88, 2, 88, 0, 0};
unsigned char MCmsg3[8] = {0, 0, 0, 0, 3, 132, 0, 0};
unsigned char MCmsg4[8] = {1, 0, 0, 0, 0, 0, 0, 0};
unsigned char MCmsg5[8] = {0, 0, 19, 136, 0, 0, 0, 0};
unsigned char MCmsg6[8] = {3, 32, 3, 32, 3, 32, 3, 32};
unsigned char MCmsg7[8] = {7, 208, 7, 208, 7, 208, 7, 208};
unsigned char MCmsg8[8] = {0, 0, 4, 0, 0, 0, 0b10000001, 0 };
unsigned char MCmsg9[8] = {0, 0, 0, 0, 0, 0, 0, 0 };
unsigned char MCmsg10[8] = {0, 0, 0, 0, 0, 0, 0, timer0};

unsigned char BMSmsg1[8] = {0b10010100,194,99,0,20,0,119};
unsigned char BMSmsg2[8] = {0,100,1,244,1,144,0,100};
unsigned char BMSmsg3[8] = {8,152,8,152,8,152,0,1};
unsigned char BMSmsg4[4] = {50,30,40,30};
unsigned char BMSmsg5[6] = {13,172,120,128,13,22};
unsigned char BMSmsg6[6] = {13,172,120,128,13,22};

unsigned char ARmsg1[8] = {0,0,0,0,0,0,0,0};



void setup() {
  

}

void loop() {
    CAN.sendMsgBuf(0x0A0, 0, 8, MCmsg1);
    delay(1);
    CAN.sendMsgBuf(0x0A1, 0, 8, MCmsg2);
    delay(1);
    CAN.sendMsgBuf(0x0A2, 0, 8, MCmsg3);
    delay(1);
    CAN.sendMsgBuf(0x0A4, 0, 8, MCmsg4);
    delay(1);
    CAN.sendMsgBuf(0x0A5, 0, 8, MCmsg5);
    delay(1);
    CAN.sendMsgBuf(0x0A6, 0, 8, MCmsg6);
    delay(1);
    CAN.sendMsgBuf(0x0A7, 0, 8, MCmsg7);
    delay(1);
    CAN.sendMsgBuf(0x0AA, 0, 8, MCmsg8);
    delay(1);
    CAN.sendMsgBuf(0x0AB, 0, 8, MCmsg9);
    delay(1);
    CAN.sendMsgBuf(0x0AC, 0, 8, MCmsg10);
    delay(1);
    CAN.sendMsgBuf(0x001, 0, 8, BMSmsg1);
    delay(1);
    CAN.sendMsgBuf(0x002, 0, 8, BMSmsg2);
    delay(1);
    CAN.sendMsgBuf(0x003, 0, 8, BMSmsg3);
    delay(1);
    CAN.sendMsgBuf(0x004, 0, 4, BMSmsg4);
    delay(1);
    CAN.sendMsgBuf(0x005, 0, 6, BMSmsg5);
    delay(1);
    CAN.sendMsgBuf(0x006, 0, 6, BMSmsg6);
    delay(1);
    CAN.sendMsgBuf(0x020, 0, 8, ARmsg1);
    delay(1);
    
    
    
    

}
