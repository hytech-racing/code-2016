#include <SPI.h>
#include <stdint.h>
#include "mcp_can.h"
#include "command_message.h"
#include "mc.h"

//EV DC - Electric Vehicle Driver Controls

MCP_CAN CAN(SPI_CS_PIN);

void activate_brake_lights() {
  /* analogWrite(BRAKE_LIGHTS_PIN, 100500); -- placeholder */
}

float get_input_voltage(int pin, float connected_voltage) {
    return /*analogRead*/(pin) * (connected_voltage / 1023.0);
}

void send_can_message(int id, unsigned char *message) {
    if (CAN.sendMsgBuf(id, 0, 8, message) != CAN_OK)
      Serial.println("Error sending!");
}

void send_can_message(can_message message) {
    send_can_message(message.get_id(), message.get_data().bytes);
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
  send_can_message(command_message(0, 0, false, true, false, 0)); // enable inverter
  send_can_message(command_message(0, 0, false, false, false, 0)); // disable inverter again
  
  send_can_message(command_message(0, 0, false, true, false, 0)); // enable inverter
  
  bool inverter_enabled = false;
  bool init_confirmed = false;
  // wait for inverter to turn on
  while (!inverter_enabled || !init_confirmed) {
    unsigned char len = 0; //Length of received message
    unsigned char buf[8];  //Where the received message will be stored
    
    if(CAN.checkReceive() == CAN_MSGAVAIL) { //Checks if message is available
      CAN.readMsgBuf(&len, buf); //If so, stores length & value
      // You have to readMsgBuf before you can get the ID of the message
      
      if (CAN.getCanId() == MC_INT_STATES_MESSAGE_ID) // this has the inverter data
        inverter_enabled = buf[5] % 2; // bit 0 of byte 5 as per specs is the inverter state
      else if (CAN.getCanId() == MC_INT_STATES_MESSAGE_ID) // init command from main arduino
        init_confirmed = buf[0] % 2; // or WHATEVER, NEED DATA ON THAT!*****************************************************************************
    }
  }
}

void loop() {
  bool use_regen = false;
  unsigned char len = 0; //Length of received message
  unsigned char buf[8];  //Where the received message will be stored
    
  float brake_input_voltage = get_input_voltage(0xA0, 5.0),
        accel_input_voltage_1 = get_input_voltage(0xA1, 5.0),
        accel_input_voltage_2 = get_input_voltage(0xA2, 5.0); // two voltages needed to compare

  if (accel_input_voltage_1 - accel_input_voltage_2 > MAX_VOLTAGE_DIFFERENCE) {
      return;// and kill the car, because wtf
  }
  
  if (brake_input_voltage > BRAKE_MAX_INPUT_VOLTAGE * 0.1) // if brake is larger than 10% of max
    activate_brake_lights();

  send_can_message(generate_command_message(
      (accel_input_voltage_1 + accel_input_voltage_2) / 2, brake_input_voltage, use_regen));
      
  if(CAN.checkReceive() == CAN_MSGAVAIL) { //Checks if message is available
    CAN.readMsgBuf(&len, buf); //If so, stores length & value
    
    if (CAN.getCanId() == MC_MOTOR_POS_INFO_MESSAGE_ID) {
      can_message::command_data received;
      memcpy(received.bytes, buf, 8); // copy buf to received.bytes
      uint16_t angular_velocity = received.shorts[1]; // bytes 2,3 as per spec
      float linear_velocity = 2 * MATH_PI * WHEEL_RADIUS * angular_velocity * (60/1000); // this is in kmph
      
      if (linear_velocity > 5)
        use_regen = true; // regen only if reported speed is above 5 kmph
    } else if (CAN.getCanId() == MC_INT_STATES_MESSAGE_ID && !(buf[0] % 2)) // shutdown command from main arduino or WHATEVER, NEED DATA ON THAT!*****************************************************************************
      return;
  }
}
