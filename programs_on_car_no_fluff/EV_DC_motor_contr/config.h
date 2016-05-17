#ifndef CONFIG_H
#define CONFIG_H

#define MATH_PI 3.14159

#define WHEEL_RADIUS 1

#define TEETH_MOTOR 16 // INCORRECT

#define TEETH_WHEEL 35 // CORRECT

#define SPI_CS_PIN 10

#define BRAKE_LIGHTS_PIN 2

#define TWELVE_ON_PIN A4 // pin 3 on the extra EVDC pins connector

#define CAN_RATE CAN_500KBPS

#define COMMAND_MESSAGE_ID 0x252

#define MAIN_COMMAND_MESSAGE_ID 0x0C0 // TODO!

#define MESSAGE_FROM_MAIN_ARDUINO 0x08C

#define MC_INT_STATES_MESSAGE_ID 0x0AA

#define MC_MOTOR_POS_INFO_MESSAGE_ID 0x0A5

#define BMS_CURRENT_MESSAGE 0x02

//#define CALCULATE_CURRENT_QUADRATIC

#define BRAKE_SUBTRACT_COEFF 1

#define BRAKE_MAX_INPUT_VOLTAGE 5.0

#define BRAKE_LIGHT_THRESHOLD 2.5

#define MAX_REGEN_CURRENT 40

#define MAX_VOLTAGE_DIFFERENCE 5.0 // 10% I believe, but only for formula SAE

#define BRAKE_PEDAL A5 // DESPITE SCHEMATIC THESE ARE CORRECT (actually, no, they were wrong...)

#define ACCEL_PEDAL_1 A6

#define ACCEL_PEDAL_2 A7

#define BRAKE_ALL_VAL 2.66

#define BRAKE_NO_VAL 2.58

#define ACC1_ALL_VAL 2.03

#define ACC1_NO_VAL 2.64

#define ACC2_ALL_VAL 0.93

#define ACC2_NO_VAL 0.43

#define DEBUG_ON 42 // if more than 0, then debug mode is on

#define DEBUG_PRINT_DELAY 500  // print debug stuff every 500 ms

#define MC_SENDER_DELAY 25 // 40 Hz for motor controller

#define MAIN_SENDER_DELAY 100 // 10 Hz for main state report

#define MAIN_TIMEOUT_LIMIT 2000 // 2 seconds for the main timeout

#define MC_TIMEOUT_LIMIT 2000 // 2 seconds for the motor controller

#endif
