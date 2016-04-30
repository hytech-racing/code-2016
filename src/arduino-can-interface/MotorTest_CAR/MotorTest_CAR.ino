

#include <mcp_can.h>
#include <math.h>
#include <SPI.h>
#include <string.h>
#include "config.h"
#include "maisc_functions.h"
#include "pinAndErrorDefinitions.h"

boolean getMultiplexerDigital(char selection) {
  digitalWrite(MULTIPLEXER_SELECT_0, LOW);
  digitalWrite(MULTIPLEXER_SELECT_1, LOW);
  digitalWrite(MULTIPLEXER_SELECT_2, LOW);
  digitalWrite(MULTIPLEXER_SELECT_3, LOW);
  
  if((selection & 0x01) == 0x01) {
    digitalWrite(MULTIPLEXER_SELECT_0, HIGH);
  }
  if((selection & 0x02) == 0x02) {
    digitalWrite(MULTIPLEXER_SELECT_1, HIGH);
  }
  if((selection & 0x04) == 0x04) {
    digitalWrite(MULTIPLEXER_SELECT_2, HIGH);
  }
  if((selection & 0x08) == 0x08) {
    digitalWrite(MULTIPLEXER_SELECT_3, HIGH);
  }
  delayMicroseconds(1) ; //for propagation delay
  return digitalRead(MULTIPLEXER_INPUT);
}


boolean shutdownButton() {
  return !getMultiplexerDigital(SHUTDOWN_BUTTON_SELECT);
}

int enabled = 0;
unsigned char lockout[8] = {0,0,0,0,0,0,0,0};
unsigned char enable[8] = {0,0,0,0,0,1,0,0};
unsigned char msg[8] = {0,0,0,0,1,1,0,0};
float torque;


MCP_CAN CAN(10);         

void setup() {
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

void loop() {
  if (!enabled) {
    CAN.sendMsgBuf(0x0C0, 0, 8, lockout);
    Serial.println("inverter disable");
    char entry;
    entry = Serial.read();
    if (shutdownButton() || entry == 's') {
      CAN.sendMsgBuf(0x0C0, 0, 8, enable);
      torque = 0;
      Serial.println("enabled");
      enabled = 1;
    }
    delay(50);
  } else {
    char entry = Serial.read();
    if (entry == 'd') {
      CAN.sendMsgBuf(0x0C0, 0, 8, enable);
      Serial.println("disabled");
      enabled = 0;
    }
    if (shutdownButton() || entry == 'e') {
      CAN.sendMsgBuf(0x0C0, 0, 8, lockout);
    }
    float brake_input_voltage = get_input_voltage(BRAKE_PEDAL, 5.0),
       accel_input_voltage_1 = get_input_voltage(ACCEL_PEDAL_1, 5.0),
       accel_input_voltage_2 = get_input_voltage(ACCEL_PEDAL_2, 5.0); // two voltages needed to compare
    brake_input_voltage = mapFloat(brake_input_voltage, BRAKE_NO_VAL, BRAKE_ALL_VAL, 0, 5.0); // map recieved voltages to true voltages
    accel_input_voltage_1 = mapFloat(brake_input_voltage, ACC1_NO_VAL, ACC1_ALL_VAL, 0, 5.0);
    accel_input_voltage_2 = mapFloat(brake_input_voltage, ACC2_NO_VAL, ACC2_ALL_VAL, 0, 5.0);
    if(accel_input_voltage_1 < 0) {
      accel_input_voltage_1 = 0;
    }
    if(accel_input_voltage_2 < 0) {
      accel_input_voltage_2 = 0;
    }
    if (brake_input_voltage > BRAKE_MAX_INPUT_VOLTAGE * 0.1) { // if brake is larger than 10% of max
      activate_brake_lights();
    }
    else {
      deactivate_brake_lights();
    }
    float percent = (accel_input_voltage_1 + accel_input_voltage_2) / 5;
    torque = compute_torque((accel_input_voltage_1 + accel_input_voltage_2) / 2, brake_input_voltage, true);


    memcpy(&msg[0], &torque, sizeof(short));
    Serial.print(msg[0]);
    Serial.print(" ");
    Serial.print(msg[1]);
    CAN.sendMsgBuf(0x0C0, 0, 8, msg);
    delay(50);
  }
}
