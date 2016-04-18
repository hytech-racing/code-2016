#ifndef PINDEFINITIONS
#define PINDEFINITIONS

#define BMS_TIMED_OUT 1
#define EVDC_TIMED_OUT 2
#define MC_TIMED_OUT 3
#define AR_TIMED_OUT 4
#define LOW_SOC 5
#define VERY_LOW_SOC 6
#define HIGH_BATT_TEMP 7

#define CELL_CRITICAL_LOW 9
#define TOO_MUCH_CURRENT 10
#define HIGH_PHASE_TEMP 11 // IGBT modules overheat
#define HIGH_MOTOR_TEMP 12
#define EVDC_PEDAL_ERROR 13 // 13 to 18
#define AR_BASE_ERROR 19 // 19 to 25 - 20 is overheat, 21 is too low, 22 is heat warning, 23 is low warning
#define IMD_BASE_ERROR 26
#define COOLANT_WARNING_TEMP 27
#define COOLANT_OVER_TEMP 28
#define TWELVE_WARNING_TEMP 29
#define TWELVE_OVER_TEMP 30
#define FIVE_OVER_TEMP 31
#define FIVE_WARNING_TEMP 32
#define FIVE1_VOLTAGE_DIP 33
#define FIVE2_VOLTAGE_DIP 34
#define TWENTYFOUR_WARNING_TEMP 35
#define TWENTYFOUR_OVER_TEMP 36

#define COOLANT_TEMPERATURE_WARNING 60
#define MAXIMUM_COOLANT_TEMPERATURE 80
#define WARNING_DCDC_TEMP 60
#define MAX_DCDC_TEMP 80




/*
okay, so the software_shutdown_control is turned on. then, the software_pushbutton_control is pulsed on for a second or so. then, AIRdcdc is turned on or off to turn AIRs on or off

*/
#define readyToDriveSound  0 // good - ready to derp sound?
#define readyToDerpSound 0
#define AirDCDC  1
#define AIRdcdc  1
#define software_shutdown_control  2 //OKAY
#define software_pushbutton_control 3 //GOOD
#define start_LED 4 
#define fan_control 5 //GOOD
#define pump_control 6 //GOOD
#define IMD_LED 7
#define BMS_LED 8
#define MULTIPLEXER_SELECT_0 9 //GOOD
// 10,11,and 12, and 13 are SPI for CAN
#define MULTIPLEXER_INPUT A4
#define CONTROL_11 A3 //GOOD
#define CONTROL_10 A5 //GOOD
#define IMDpin A7               // GOOD - needs to sub in for A6
#define twenty_four_thermistor A6  // GOOD - needs to sub in for A7 
#define MULTIPLEXER_SELECT_3 A2 // GOOD
#define MULTIPLEXER_SELECT_2 A1 // GOOD - cant be used as digital pin, need to change to A3
#define MULTIPLEXER_SELECT_1 A0 // GOOD - cant be used as digital pin, need to change to A4

// mutiplexer definitions

#define SHUTDOWN_BUTTON_SELECT 0
#define BOOST_BUTTON_SELECT 1
#define CYCLE_BUTTON_SELECT 2
#define TOGGLE_BUTTON_SELECT 3
#define FIVE_SUPPLY_CHECK_SELECT 4
#define TWELVE_THERMISTOR1_SELECT 5
#define TWELVE_THERMISTOR2_SELECT 6
#define FIVE_THERMISTOR_SELECT 7
#define VCC_AIR_DIVIDER_SELECT 8
#define RELAY_THERMISTOR_SELECT 9
#define AIR_UNREGULATED_DIVIDER 10
#define COOLANT_SENSOR_1_SELECT 11
#define COOLANT_SENSOR_2_SELECT 12
#define COOLANT_SENSOR_3_SELECT 13
#define IMD_PWM_LOW_SELECT 14
#define IMD_PWM_HIGH_SELECT 15

#endif
