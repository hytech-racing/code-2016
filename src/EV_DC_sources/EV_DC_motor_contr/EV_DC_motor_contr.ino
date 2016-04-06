#include <SPI.h>
#include <stdint.h>
#include "mcp_can.h"
#include "command_message.h"
#include "mc.h"

//EV DC - Electric Vehicle Driver Controls

bool use_regen = false; // I know you hate global variables but I had to do this :'(
bool total_shutdown = false;
bool run_normal = true;
long MAIN_TIMEOUT = 0;
int MAIN_TIMEOUT_LIMIT = 2000; // two seconds

MCP_CAN CAN(SPI_CS_PIN);

void activate_brake_lights() {
  digitalWrite(BRAKE_LIGHTS_PIN, HIGH); -- placeholder */
}

float get_input_voltage(int pin, float connected_voltage) {
    return analogRead(pin) * (connected_voltage / 1023.0);
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
  while (CAN.begin(CAN_RATE) != CAN_OK) { //Communication rate is 500Kbps as per specs
    Serial.println("CAN BUS Shield init FAIL");
    Serial.println("Init CAN BUS Shield again");
    delay(100);
  }
  
  Serial.println("CAN BUS Shield Init OK");
  
  bool good_for_launch = false;
  bool inverter_enabled = false;
  bool init_confirmed = false;
  
  while(!good_for_launch) {
    if(CAN.checkReceive() == CAN_MSGAVAIL) { //Checks if message is available
      CAN.readMsgBuf(&len, buf); //If so, stores length & value
      // You have to readMsgBuf before you can get the ID of the message
      
      if (CAN.getCanId() == MC_INT_STATES_MESSAGE_ID) // this has the inverter data
        inverter_enabled = buf[5] % 2; // bit 0 of byte 5 as per specs is the inverter state
      else if (CAN.getCanId() == MAIN_MESSAGE_GET) // init command from main arduino
        init_confirmed = (buf[0]  == 42); // or WHATEVER, NEED DATA ON THAT!*****************************************************************************
    }
  

  
  bool inverter_enabled = false;
  bool init_confirmed = false;
  // wait for inverter to turn on
  while (!inverter_enabled || init_confirmed) { // while it is not enabled but main arduino initialization is finished, send out enable commands
    
  send_can_message(command_message(0, 0, false, false, false, 0)); // Send out inverter disable command to release lockout.
  send_can_message(command_message(0, 0, false, true, false, 0)); // enable inverter
  send_can_message(command_message(0, 0, false, false, false, 0)); // disable inverter again
  
  send_can_message(command_message(0, 0, false, true, false, 0)); // enable inverter
    
    unsigned char len = 0; //Length of received message
    unsigned char buf[8];  //Where the received message will be stored
    
    if(CAN.checkReceive() == CAN_MSGAVAIL) { //Checks if message is available
      CAN.readMsgBuf(&len, buf); //If so, stores length & value
      // You have to readMsgBuf before you can get the ID of the message
      
      if (CAN.getCanId() == MC_INT_STATES_MESSAGE_ID) // this has the inverter data
        inverter_enabled = buf[5] % 2; // bit 0 of byte 5 as per specs is the inverter state
      else if (CAN.getCanId() == MAIN_MESSAGE_GET) // init command from main arduino
        init_confirmed = (buf[0] == 42); // or WHATEVER, NEED DATA ON THAT!*****************************************************************************
    }
  }
}

void loop() {
  
  unsigned char len = 0; //Length of received message
  unsigned char buf[8];  //Where the received message will be stored
    
  float brake_input_voltage = get_input_voltage(BRAKE_PEDAL, 5.0),
        accel_input_voltage_1 = get_input_voltage(ACCEL_PEDAL_1, 5.0),
        accel_input_voltage_2 = get_input_voltage(ACCEL_PEDAL_2, 5.0); // two voltages needed to compare

  if (abs(accel_input_voltage_1 - accel_input_voltage_2) > MAX_VOLTAGE_DIFFERENCE) {
      accel_input_voltage_1 = 0;// and kill the car, because wtf
      accel_input_voltage_2 = 0;
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
      else
        use_regen = false;
    } 
    else if (CAN.getCanId() == MAIN_MESSAGE_GET) { // shutdown command from main arduino or WHATEVER, NEED DATA ON THAT!*****************************************************************************
    MAIN_TIMEOUT = millis() + MAIN_TIMEOUT_LIMIT;
      if(buf[0] == 42) { // everything is fine
        run_normal = !total_shutdown;
      }
      
      else if(buf[0] == 25) { // a critical error that is unrecoverable has occurred
        total_shutdown = true;
      }
      
      else if(buf[0] == 64) {
        run_normal = false;
      }
        
    
    }
}
