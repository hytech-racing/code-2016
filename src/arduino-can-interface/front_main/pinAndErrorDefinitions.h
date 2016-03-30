#define BMS_TIMED_OUT 1
#define EVDC_TIMED_OUT 2
#define MC_TIMED_OUT 3
#define AR_TIMED_OUT 4
#define LOW_SOC 5
#define VERY_LOW_SOC 6
#define HIGH_BATT_TEMP 7

#define CELL_CRITICAL_LOW 9
#define TOO_MUCH_CURRENT 10
#define HIGH_PHASE_TEMP 11
#define HIGH_MOTOR_TEMP 12
#define EVDC_BASE_ERROR 13 // 13 to 18
#define AR_BASE_ERROR 19 // 19 to 25
#define IMD_BASE_ERROR 26
#define COOLANT_WARNING_TEMP 27
#define COOLANT_OVER_TEMP 28
#define TWELVE_WARNING_TEMP 29
#define TWELVE_OVER_TEMP 30
#define FIVE_OVER_TEMP 31
#define FIVE_OVER_TEMP 32
#define FIVE1_VOLTAGE_DIP 33
#define FIVE2_VOLTAGE_DIP 34

#define COOLANT_TEMPERATURE_WARNING 60
#define MAXIMUM_COOLANT_TEMPERATURE 80
#define WARNING_DCDC_TEMP 60
#define MAX_DCDC_TEMP 80




/*
okay, so the software_shutdown_control is turned on. then, the software_pushbutton_control is pulsed on for a second or so. then, AIRdcdc is turned on or off to turn AIRs on or off

*/
const int readyToDriveSound = 0; // ready to derp sound?
const int AirDCDC = 1;
const int AIRdcdc = 1;
const int software_shutdown_control = 2;
const int software_pushbutton_control = 3;
const int start_LED = 4;
const int fan_control = 5;
const int pump_control = 6;
const int IMD_LED = 7;
const int BMS_LED = 8;
const int MULTIPLEXER_SELECT_0 = 9;
// 10,11,and 12, and 13 are SPI for CAN
const int MULTIPLEXER_INPUT A0;
const int CONTROL_11 A1;
const int CONTROL_10 A2;
const int IMDpin A3;
const int five_supply_check_2 = A4;
const int MULTIPLEXER_SELECT_3 = A5;
const int MULTIPLEXER_SELECT_2 = A6;
const int MULTIPLEXER_SELECT_1 = A7;

// mutiplexer definitions

const char SHUTDOWN_BUTTON_SELECT = 0;
const char BOOST_BUTTON_SELECT = 1;
const char CYCLE_BUTTON_SELECT = 2;
const char TOGGLE_BUTTON_SELECT = 3;
const char FIVE_SUPPLY_CHECK_SELECT = 4;
const char TWELVE_THERMISTOR1_SELECT = 5;
const char TWELVE_THERMISTOR2_SELECT = 6;
const char FIVE_THERMISTOR_SELECT = 7;
const char VCC_AIR_DIVIDER_SELECT = 8;
const char RELAY_THERMISTOR_SELECT = 9;
const char AIR_UNREGULATED_DIVIDER = 10;
const char COOLING_SENSOR_1_SELECT = 11;
const char COOLING_SENSOR_2_SELECT = 12;
const char COOLING_SENSOR_3_SELECT = 13;
const char IMD_PWM_LOW_SELECT = 14;
const char IMD_PWM_HIGH_SELECT = 15;

