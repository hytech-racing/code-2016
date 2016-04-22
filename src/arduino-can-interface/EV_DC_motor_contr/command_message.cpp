#include <math.h>
#include "command_message.h"

command_message::command_message(uint16_t torque,
    uint16_t ang_velocity, bool clockwise,
    bool enable_inverter, bool enable_discharge,
    uint16_t new_torque_limit)
{
    id = COMMAND_MESSAGE_ID;
    command_data[0] = char(torque & 0xFF); //data.shorts
    command_data[1] = char(torque>>(8));
    command_data[2] = char(ang_velocity & 0xFF);
    command_data[3] = char(ang_velocity>>(8));
    command_data[4] = clockwise;
    command_data[5] = char(enable_inverter)
        + 2 * char(enable_discharge);
    command_data[6] = char(new_torque_limit & 0xFF);
    command_data[7] = char(new_torque_limit>>(8));
}


command_message::~command_message()
{
    id = 0xC0; //default
}
//
//uint16_t command_message::get_torque()
//{
//    return data.shorts[0];
//}
//
//uint16_t command_message::get_ang_velocity()
//{
//    return data.shorts[1];
//}
//
//bool command_message::is_clockwise()
//{
//    return (data.bytes[4]) != 0;
//}
//
//bool command_message::inverter_enabled()
//{
//    return (data.bytes[5] & 0x01) != 0;
//}
//
//bool command_message::discharge_enabled()
//{
//    return (data.bytes[5] & 0x02) != 0;
//}
//
//uint16_t command_message::get_new_torque_limit()
//{
//    return data.shorts[3];
//}

command_message generate_command_message(float input_voltage, float brake_voltage, bool regen) {
    float output_current;
    int divisor;

#define CALCULATE_DIVISOR for (divisor = 1;\
    input_voltage > 0.99; input_voltage /= 10, divisor *= 10);

#if defined(CALCULATE_CURRENT_QUADRATIC) // x(2-x)
    CALCULATE_DIVISOR
    output_current = divisor * input_voltage * (2 - input_voltage);
#elif defined(CALCULATE_CURRENT_CUBIC) // x^2(3-2x)
    CALCULATE_DIVISOR
    output_current = divisor * input_voltage * input_voltage
        * (3 - 2 * input_voltage);
#elif defined(CALCULATE_CURRENT_CUBIC_TIMES_SQRT) // x^2.5(3-2x)
    CALCULATE_DIVISOR
    output_current
        = divisor * input_voltage * sqrt(input_voltage)
        * (3 - 2 * input_voltage);
#elif defined(CALCULATE_CURRENT_QUARTIC) // x^3(2.5-1.5x)
    CALCULATE_DIVISOR
    output_current
        = divisor * input_voltage * input_voltage * input_voltage
        * (2.5 - 1.5 * input_voltage);
#elif defined(CALCULATE_CURRENT_SQRT_QUARTIC) // x^1.5(2.5-1.5x)
    CALCULATE_DIVISOR
    output_current
        = divisor * sqrt(input_voltage * input_voltage * input_voltage)
        * (2.5 - 1.5 * input_voltage);
#elif defined(CALCULATE_CURRENT_QUuintIC) // x^4(2-x)
    CALCULATE_DIVISOR
    output_current
        = divisor * input_voltage * input_voltage
        * input_voltage * input_voltage
        * (2 - input_voltage);
#else // x
    output_current = input_voltage;
#endif
    //Serial.println(curren_coeff);
    float result_current = output_current * current_coeff * 10;
    float result_braked = result_current - BRAKE_SUBTRACT_COEFF * brake_voltage;
    
    if (!regen && result_braked < 0) {
      result_braked = 0;
    }

    return command_message(result_braked,0, false, true, false, 0);

#undef CALCULATE_DIVISOR
}
