/**
 * Victor Chen
 * 28 Feb 2016
 * code for rear arduino, send error message on condition
 */
#include <mcp_can.h>
#include <SPI.h>

// the cs pin of the version after v1.1 is default to D9
// v0.9b and v1.0 is default D10
const int SPI_CS_PIN = 9;

MCP_CAN CAN(SPI_CS_PIN);                                    // Set CS pin

int C_TH = 6;
int B_TH = 7;
int A_TH = 8;
int C_BAT = 9;
int B_BAT = 10;
int A_BAT = 11;
int ANALOG_TH = 26;
int ANALOG_BAT = 25;
void setup()
{
    pinMode(C_TH, OUTPUT);
    pinMode(B_TH, OUTPUT);
    pinMode(A_TH, OUTPUT);
    pinMode(C_BAT, OUTPUT);
    pinMode(B_BAT, OUTPUT);
    pinMode(A_BAT, OUTPUT);
    pinMode(ANALOG_TH, INPUT);
    pinMode(ANALOG_BAT, INPUT);
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
    /*Read in thermistor values and battery voltages
     */
    int TH1, TH2, TH3, TH4, TH5, TH6, TH7, TH8;
    float battery[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0}; /*the voltages the arduino reads in*/
    float actual[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0}; /*The actual voltages at each resistor bridge*/
    for (int i = 0; i < 9; i++) {
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
        case 1 :
          TH1 = analogRead(ANALOG_TH);
          battery[i] = analogRead(ANALOG_BAT) * ((float) 5 / 1023);
          actual[i] = battery[i];
        case 2 : 
          TH2 = analogRead(ANALOG_TH);
          battery[i] = analogRead(ANALOG_BAT) * ((float) 5 / 1023);
          actual[i] = battery[i] * ((10 + 10)/ 10);
        case 3 : 
          TH3 = analogRead(ANALOG_TH);
          battery[i] = analogRead(ANALOG_BAT) * ((float) 5 / 1023);
          actual[i] = battery[i] * ((20 + 10) / 10);
        case 4 :
          TH4 = analogRead(ANALOG_TH);
          battery[i] = analogRead(ANALOG_BAT) * ((float) 5 / 1023);
          actual[i] = battery[i] * ((30 + 10) / 10);
        case 5 :
          TH5 = analogRead(ANALOG_TH);
          battery[i] = analogRead(ANALOG_BAT) * ((float) 5 / 1023);
          actual[i] = battery[i] * ((39 + 10) / 10);
        case 6 :
          TH6 = analogRead(ANALOG_TH);
          battery[i] = analogRead(ANALOG_BAT) * ((float) 5 / 1023);
          actual[i] = battery[i] * (51 + 10) / 10);
        case 7 :
          TH7 = analogRead(ANALOG_TH);
          battery[i] = analogRead(ANALOG_BAT) * ((float) 5 / 1023);
          actual[i] = battery[i] * ((62 + 10) / 10);
        case 8 :
          TH8 = analogRead(ANALOG_TH);
          battery[i] = analogRead(ANALOG_BAT) * ((float) 5 / 1023);
          actual[i] = battery[i] * ((68 + 10) / 10);
      }
      /*TODO: check thermistor values */

      /*Check battery values*/
      for (int i = 1; i < 9; i++) {
        if ((actual[i] - actual[i-1]) < 2.0) {
          /*throw an error*/
        }
      }
    }
    delay(100);                       // send data per 100ms
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/