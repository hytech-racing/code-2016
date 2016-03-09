#include <SPI.h>
#include "mcp_can.h"
#include "command_message.h"
#include "mc.h"

//EV DC - Electric Vehicle Driver Controls

MCP_CAN CAN(SPI_CS_PIN);

float get_input_voltage(int pin, float connected_voltage) {
    return /*analogRead*/(pin) * (connected_voltage / 1023.0);
}

bool get_switch_input(int pin) { //down == true, I guess
    return true;
}

void send_can_message(int id, unsigned char *message) {
    if (CAN.sendMsgBuf(id, 0, 8, message) != CAN_OK)
      Serial.println("Error sending!");
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

/*
  Sets the pin Arduino uses for the CAN controller.
  Set to 9 if using a SEEEDStudio (black) shield
  Set to 10 if using a SparkFun (red) shield
*/

void setup() {
  MCP_CAN CAN(SPI_CS_PIN);
  Serial.begin(9600); //initialize serial for PC communication

  /*
    Initializes CAN controller
  */
  while (CAN.begin(CAN_RATE) != CAN_OK) { //Communication rate is 250Kbps as per specs
    Serial.println("CAN BUS Shield init FAIL");
    Serial.println("Init CAN BUS Shield again");
    delay(100);
  }
  
  Serial.println("CAN BUS Shield Init OK");
  
  send_can_message(command_message(0, 0, false, false, false, 0)); // Send out inverter disable command to release lockout.
  
  
  ev_dc_looper();
}

void loop() {
  /* unused */
}
