#include <iostream>
#include "command_message.h"

//EV DC - Electric Vehicle Driver Controls

float get_input_voltage(int pin, float connected_voltage) {
    return /*analogRead*/(pin) * (connected_voltage / 1023.0);
}

bool get_switch_input(int pin) { //down == true, I guess
    return true;
}

void send_can_message(int address, uint8_t *message) {
    // send message
}

void send_can_message(can_message message) {
    send_can_message(message.get_id(), message.get_data().bytes);
}

void ev_dc_looper() {
    while (true) {
        float brake_input_voltage = get_input_voltage(0xA0, 5.0),
            accel_input_voltage_1 = get_input_voltage(0xA1, 5.0),
            accel_input_voltage_2 = get_input_voltage(0xA2, 5.0); // two voltages needed to compare
        int switch_1 = get_switch_input(0xA3); //TODO!

        if (accel_input_voltage_1 - accel_input_voltage_2 > MAX_VOLTAGE_DIFFERENCE) {
            return;// and kill the car, because wtf
        }

        send_can_message(generate_command_message(
            (accel_input_voltage_1 + accel_input_voltage_2) / 2, brake_input_voltage, USE_REGEN));
    }
}

int main(void) {
    command_message message = generate_command_message(75, 0, false);

    for (uint8_t val : message.get_data().bytes) {
        printf("%d\n", val);
    }

    scanf("%d");

    return 0;
}