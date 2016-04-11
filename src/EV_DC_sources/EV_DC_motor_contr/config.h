#ifndef CONFIG_H
#define CONFIG_H

#define MATH_PI 3.14159

#define WHEEL_RADIUS 1

#define SPI_CS_PIN 10

#define BRAKE_LIGHTS_PIN 2

#define BRAKE_PEDAL A0

#define ACCEL_PEDAL_1 A1

#define ACCEL_PEDAL_2 A2

#define CAN_RATE CAN_500KBPS

#define COMMAND_MESSAGE_ID 0x0C0

#define MAIN_COMMAND_MESSAGE_ID 0x0C0 // TODO!

#define MC_INT_STATES_MESSAGE_ID 0x0AA

#define MC_MOTOR_POS_INFO_MESSAGE_ID 0x0A5

#define CALCULATE_CURRENT_QUADRATIC

#define BRAKE_SUBTRACT_COEFF 1

#define BRAKE_MAX_INPUT_VOLTAGE 100500

#define MAX_VOLTAGE_DIFFERENCE 0.5 // 10% I believe

#define BRAKE_ALL_VAL 5.0

#define BRAKE_NO_VAL 0

#define ACC1_ALL_VAL 5.0

#define ACC1_NO_VAL 0

#define ACC2_ALL_VAL 5.0

#define ACC2_NO_VAL 0

#define DEBUG_ON 0 // if more than 0, then debug mode is on

#define MC_SENDER_DELAY 25 // 40 Hz for motor controller

#define MAIN_SENDER_DELAY 100 // 10 Hz for main state report

#define MAIN_TIMEOUT_LIMIT 2000 // 2 seconds for the main timeout

#endif
