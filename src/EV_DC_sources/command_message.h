#ifndef COMMAND_MESSAGE_H
#define COMMAND_MESSAGE_H

#include <stdint.h>

#include "config.h"

#include "can_message.h"

const float current_coeff = 0.54f; /* N.m/amp: used to convert torque to output current */

class command_message : public can_message
{
public:
    command_message(int id,
        uint16_t torque,
        uint16_t ang_velocity, bool clockwise,
        bool enable_inverter, bool enable_discharge,
        uint16_t new_torque_limit);
    ~command_message();

    uint16_t get_torque();
    uint16_t get_ang_velocity();
    bool is_clockwise();
    bool inverter_enabled();
    bool discharge_enabled();
    uint16_t get_new_torque_limit();

protected:
    command_data data;
    int id;
};

command_message generate_command_message(float input_voltage, float brake_voltage, bool regen);

#endif