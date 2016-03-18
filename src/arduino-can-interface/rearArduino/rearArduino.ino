/**
 * Victor Chen
 * 17 Mar 2016
 * code for rear arduino, send error message on condition
 */
#include "thermistor.h"
#include <mcp_can.h>
#include <SPI.h>

// the cs pin of the version after v1.1 is default to D9
// v0.9b and v1.0 is default D10r
const int SPI_CS_PIN = 10;

MCP_CAN CAN(SPI_CS_PIN);                                    // Set CS pin

int C_TH = 3;
int B_TH = 4;
int A_TH = 5;
int C_BAT = 6;
int B_BAT = 7;
int A_BAT = 8;
//int ANALOG_TH = A7;
//int ANALOG_BAT = A6;
void setup()
{
    pinMode(C_TH, OUTPUT);
    pinMode(B_TH, OUTPUT);
    pinMode(A_TH, OUTPUT);
    pinMode(C_BAT, OUTPUT);
    pinMode(B_BAT, OUTPUT);
    pinMode(A_BAT, OUTPUT);
    pinMode(A0, INPUT); // thermistors
    pinMode(A1, INPUT); /*analog_bat*/
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

void loop()
{
    digitalWrite(C_TH, LOW);
    digitalWrite(B_TH, LOW);
    digitalWrite(A_TH, LOW);
    digitalWrite(C_BAT, LOW);
    digitalWrite(B_BAT, LOW);
    digitalWrite(A_BAT, LOW);
    /*Read in thermistor values and battery voltages
     */
    int th[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    float battery[8] = {0, 0, 0, 0, 0, 0, 0, 0}; /*the voltages the arduino reads in*/
    float actual[8] = {0, 0, 0, 0, 0, 0, 0, 0}; /*The actual voltages at each resistor bridge*/
    unsigned char msg[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    float sum[8] = {0, 0, 0, 0, 0, 0, 0, 0}; 
    for (int j = 0; j < 5; j++) {
        for (int i = 0; i < 8; i++) {

            digitalWrite(C_TH, LOW);
            digitalWrite(B_TH, LOW);
            digitalWrite(A_TH, LOW);
            digitalWrite(C_BAT, LOW);
            digitalWrite(B_BAT, LOW);
            digitalWrite(A_BAT, LOW);
      
            if ((i & 4) != 0) {
                digitalWrite(C_TH, HIGH);
                digitalWrite(C_BAT, HIGH);
            }
            if ((i & 2) != 0) {
                digitalWrite(B_TH, HIGH);
                digitalWrite(B_BAT, HIGH);
            }
            if ((i & 1) != 0) {
                digitalWrite(A_TH, HIGH);
                digitalWrite(A_BAT, HIGH);
            }
            switch(i) {
                case 0 :
                    th[0] = analogRead(A0);
                    battery[7 - i] = analogRead(A1) * ((float) 5 / 1023);
                    actual[7 - i] = battery[7 - i] * (3.2955/3.5);
                    sum[7 - i] += actual[7 - i];
                    break;
                case 1: 
                    th[1] = analogRead(A0);
                    battery[7 - i] = analogRead(A1) * ((float) 5 / 1023);
                    actual[7 - i] = battery[7 - i] * ((10 + 10)/ 10.0) * (6.589/7.00);
                    sum[7 - i] += actual[7 - i];
                    break;
                case 2 : 
                    th[2] = analogRead(A0);
                    battery[7 - i] = analogRead(A1) * ((float) 5 / 1023);
                    actual[7 - i] = battery[7 - i] * ((20 + 10)/ 10.0) * (9.885/10.44);
                    sum[7 - i] += actual[7 - i];
                    break;
                case 3 :
                    th[3] = analogRead(A0);
                    battery[7 - i] = analogRead(A1) * ((float) 5 / 1023);
                    actual[7 - i] = battery[7 - i] * ((30 + 10)/ 10.0) * (13.180/13.06);
                    sum[7 - i] += actual[7 - i];
                    break;
                case 4 :
                    th[4] = analogRead(A0);
                    battery[7 - i] = analogRead(A1) * ((float) 5 / 1023);
                    actual[7 - i] = battery[7 - i] * ((39 + 10) / 10.0) * (16.447/17.50);
                    sum[7 - i] += actual[7 - i];
                    break;
                case 5 :
                    th[5] = analogRead(A0);
                    battery[7 - i] = analogRead(A1) * ((float) 5 / 1023);
                    actual[7 - i] = battery[7 - i] * ((51 + 10) / 10.0) * (19.772/20.73);
                    sum[7 - i] += actual[7 - i];
                    break;
                case 6 :
                    th[6] = analogRead(A0);
                    battery[7 - i] = analogRead(A1) * ((float) 5 / 1023);
                    actual[7 - i] = battery[7 - i] * ((62 + 10) / 10.0) * (23.068/24.18);
                    sum[7 - i] += actual[7 - i];
                    break;
                case 7 :
                    th[7] = analogRead(A0);
                    battery[7 - i] = analogRead(A1) * ((float) 5 / 1023);
                    actual[7 - i] = battery[7 - i] * ((68 + 10) / 10.0) * (26.364/27.54);
                    sum[7 - i] += actual[7 - i];
                    break;
            }
        }
        //finished with one entire reading, wait 20 ms, do until 5 readings
        delay(20);
    }
    /*TODO: check thermistor values */
    for (int i = 0; i < 5; i++) {
        if (checkThermistor(10, th[i]) >= 100) {
            msg[1] = 2;
        } else if (checkThermistor(10, th[i]) >= 80) {
            msg[1] = 1;
        }
    }
    /*Check battery values*/
    for (int i = 0; i < 7; i++) {
        if ((actual[i + 1] - actual[i]) < 2.8) {
            msg[0] = 2;
        } else if ((actual[i + 1] - actual[i]) < 3.0) {
            msg[0] = 1;
        }
    }
    // bit 7 of message: battery error
    // bit 6 of message: thermistor error
    CAN.sendMsgBuf(0x2A2, 0, 8, msg);

    /*PRINT STATEMENTS FOR DEBUGGING*/
//    for(int i = 0; i < 8; i++) {
//      Serial.print("Thermistor ");
//      Serial.print(i+1);
//      Serial.print(" : ");
//      Serial.println(th[i]);
//    }
//    for(int i = 0; i<8;i++) {
//      Serial.print("Battery: ");
//      Serial.print(i + 1);
//      Serial.print(" : ");
//      Serial.println(battery[i]);
//    }
//    for(int i = 0; i < 8; i++) {
//      Serial.print("Battery ");
//      Serial.print(i + 1);
//      Serial.print(" : ");
//      Serial.println(actual[i]);
//    }
//    for(int i = 0; i < 8; i++) {
//      Serial.print("Battery ");
//      Serial.print(i+1);
//      Serial.print(" Avg: ");
//      Serial.println(sum[i]/5);
//    }
//    for (int i = 0; i < 8; i++) {
//      Serial.print(msg[i]);
//      Serial.print(" ");
//    }
//    Serial.println();
//    Serial.println("**************************");
    delay(10);                       // send data per 100ms
}

//Thermistor 1 : 0
//Thermistor 2 : 1
//Thermistor 3 : 0
//Thermistor 4 : 0
//Thermistor 5 : 0
//Thermistor 6 : 0
//Thermistor 7 : 0
//Thermistor 8 : 0
//Battery: 1 : 3.53
//Battery: 2 : 3.37
//Battery: 3 : 3.39
//Battery: 4 : 3.57
//Battery: 5 : 3.08
//Battery: 6 : 3.48
//Battery: 7 : 3.50
//Battery: 8 : 3.50
//Battery 1 : 27.54 26.364
//Battery 2 : 24.18 23.068
//Battery 3 : 20.73 19.772
//Battery 4 : 17.50 16.447
//Battery 5 : 13.06 13.180
//Battery 6 : 10.44 9.885
//Battery 7 : 7.00  6.589
//Battery 8 : 3.50  3.2955



/*********************************************************************************************************
  END FILE
*********************************************************************************************************/


