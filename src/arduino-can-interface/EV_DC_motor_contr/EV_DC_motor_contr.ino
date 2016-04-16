


#include <SPI.h>
#include "mcp_can.h"
#include <stdint.h>
#include "config.h"

#include "maisc_functions.h"
#include "arduinoMain.h"
#include "command_message.h"
#include "mc.h"


//EV DC - Electric Vehicle Driver Controls

bool use_regen = false; // I know you hate global variables but I had to do this :'(
bool total_shutdown = false;
bool run_normal = true;
long MC_TIMEOUT = 0;
long MAIN_TIMEOUT = 0;
long MAIN_SENDER_TIMER = 0;
long MC_SENDER_TIMER = 0;
long debugPrintTimer = 0;



MCP_CAN CAN(SPI_CS_PIN);
/*
  Sets the pin Arduino uses for the CAN controller.
  Set to 9 if using a SEEEDStudio (black) shield
  Set to 10 if using a SparkFun (red) shield
*/

void send_can_message(int id, unsigned char *message) {
    if (CAN.sendMsgBuf(id, 0, 8, message) != CAN_OK)
      Serial.println("Error sending!");
}

void send_can_message(can_message message) {
    send_can_message(message.get_id(), message.get_data().bytes);
}

















void setup() { // BEGIN SETUP~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  //MCP_CAN CAN(SPI_CS_PIN);
  unsigned char len = 0;
  unsigned char buf[8];
 long setupTimer = 0; // all-purpose setup timer

  
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
    
    float brake_input_voltage = get_input_voltage(BRAKE_PEDAL, 5.0); //get brake value for main arduino
    
    if(DEBUG_ON > 0 && millis() > debugPrintTimer) {
      float accel_input_voltage_1 = get_input_voltage(ACCEL_PEDAL_1, 5.0),
            accel_input_voltage_2 = get_input_voltage(ACCEL_PEDAL_2, 5.0); // two voltages needed to compare
      
      Serial.println("raw: brake, then acc_1, then acc_2");
      Serial.println(brake_input_voltage);
      Serial.println(accel_input_voltage_1);
      Serial.println(accel_input_voltage_2);
      debugPrintTimer += DEBUG_PRINT_DELAY;
    }
    
    brake_input_voltage = mapFloat(brake_input_voltage, BRAKE_NO_VAL, BRAKE_ALL_VAL, 0, 5.0);
    
    setupTimer = millis() + 50;
    while(setupTimer > millis()){ //listen to can for 50 milisnencends
      if(CAN.checkReceive() == CAN_MSGAVAIL) { //Checks if message is available
        CAN.readMsgBuf(&len, buf); //If so, stores length & value
        // You have to readMsgBuf before you can get the ID of the message
        
        if (CAN.getCanId() == MC_INT_STATES_MESSAGE_ID) // this has the inverter data
          inverter_enabled = buf[5] % 2; // bit 0 of byte 5 as per specs is the inverter state
        else if (CAN.getCanId() == AM::MAIN_MESSAGE_GET) // init command from main arduino
          init_confirmed = (buf[0]  == 42); // or WHATEVER, NEED DATA ON THAT!**
      }
    }
    
    send_can_message(command_message(0, 0, false, false, false, 0)); // Send at least one message to prevent MC shutdown
  
    unsigned char message[8];
    message[1] = char(brake_input_voltage * 20.0); // brakes from 0 to 100
    send_can_message(AM::MAIN_MESSAGE_SEND, message);
       
    bool inverter_enabled = false;
    bool init_confirmed = false;
    // wait for inverter to turn on
    while (!inverter_enabled && init_confirmed) { // while it is not enabled but main arduino initialization is finished, send out enable commands
      
      send_can_message(command_message(0, 0, false, false, false, 0)); // Send out inverter disable command to release lockout.
      send_can_message(command_message(0, 0, false, true, false, 0)); // enable inverter
      send_can_message(command_message(0, 0, false, false, false, 0)); // disable inverter again
      
      send_can_message(command_message(0, 0, false, true, false, 0)); // enable inverter
      
      brake_input_voltage = mapFloat(get_input_voltage(BRAKE_PEDAL, 5.0), BRAKE_NO_VAL, BRAKE_ALL_VAL, 0, 5.0);
      
      unsigned char messageOther[8];
      message[1] = int(brake_input_voltage * 20.0); // brakes from 0 to 100
      send_can_message(AM::MAIN_MESSAGE_SEND, messageOther);
      
      unsigned char len = 0; //Length of received message
      unsigned char buf[8];  //Where the received message will be stored
      
      setupTimer = millis() + 50;
      while(setupTimer > millis()){ //listen to can for 50 millisecdons
        if(CAN.checkReceive() == CAN_MSGAVAIL) { //Checks if message is available
          CAN.readMsgBuf(&len, buf); //If so, stores length & value
          // You have to readMsgBuf before you can get the ID of the message
          
          if (CAN.getCanId() == MC_INT_STATES_MESSAGE_ID) // this has the inverter data
            inverter_enabled = buf[5] % 2; // bit 0 of byte 5 as per specs is the inverter state
          else if (CAN.getCanId() == AM::MAIN_MESSAGE_GET) // init command from main arduino
            init_confirmed = (buf[0]  == 42);
        }
      }
    } // end of "while inverter not enabled and init confirmed"
  }// end of "while not good for launch"
}// end of "void setup"






















void loop() { // BEGIN LOOP~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  
  unsigned char len = 0; //Length of received message
  unsigned char buf[8];  //Where the received message will be stored
  unsigned char mainMessage[8];
  boolean pedal_error = false;
  
  
  float brake_input_voltage = get_input_voltage(BRAKE_PEDAL, 5.0),
       accel_input_voltage_1 = get_input_voltage(ACCEL_PEDAL_1, 5.0),
       accel_input_voltage_2 = get_input_voltage(ACCEL_PEDAL_2, 5.0); // two voltages needed to compare
  
  if(DEBUG_ON > 0 && millis() > debugPrintTimer) {
        
    Serial.println("raw: brake, then acc_1, then acc_2");
    Serial.println(brake_input_voltage);
    Serial.println(accel_input_voltage_1);
    Serial.println(accel_input_voltage_2);
  }
  
        
  brake_input_voltage = mapFloat(brake_input_voltage, BRAKE_NO_VAL, BRAKE_ALL_VAL, 0, 5.0); // map recieved voltages to true voltages
  accel_input_voltage_1 = mapFloat(brake_input_voltage, ACC1_NO_VAL, ACC1_ALL_VAL, 0, 5.0);
  accel_input_voltage_2 = mapFloat(brake_input_voltage, ACC2_NO_VAL, ACC2_ALL_VAL, 0, 5.0);
  
  if(accel_input_voltage_1 < 0) {
    accel_input_voltage_1 = 0;
  }
  if(accel_input_voltage_2 < 0) {
    accel_input_voltage_2 = 0;
  }
    
  if(!run_normal) {   
    accel_input_voltage_1 = 0;
    accel_input_voltage_2 = 0; 
  }
  else {
    mainMessage[0] = 42; // EVDC is running normal
  }

  if (abs(accel_input_voltage_1 - accel_input_voltage_2) > MAX_VOLTAGE_DIFFERENCE) {
      accel_input_voltage_1 = 0;// and kill the car, because wtf
      accel_input_voltage_2 = 0;
      pedal_error = true;
  }
  
  
  if (brake_input_voltage > BRAKE_MAX_INPUT_VOLTAGE * 0.1) { // if brake is larger than 10% of max
    activate_brake_lights();
  }
  else {
    deactivate_brake_lights();
  }
    
  if(millis() > MC_SENDER_TIMER){
    send_can_message(generate_command_message((accel_input_voltage_1 + accel_input_voltage_2) / 2, brake_input_voltage, use_regen));
    MC_SENDER_TIMER += MC_SENDER_DELAY;
  }
  
  
  if(pedal_error) {
    mainMessage[2] = 1;
  }
  mainMessage[1] = int(brake_input_voltage * 20.0); // brakes from 0 to 100
  
  
  if(millis() > MAIN_SENDER_TIMER){
    send_can_message(AM::MAIN_MESSAGE_SEND, mainMessage);
    MAIN_SENDER_TIMER += MAIN_SENDER_DELAY;
  }
    
  
  
  
      
  if(CAN.checkReceive() == CAN_MSGAVAIL) { //Checks if message is available
    CAN.readMsgBuf(&len, buf); //If so, stores length & value
    
    if (CAN.getCanId() == MC_MOTOR_POS_INFO_MESSAGE_ID) {
      MC_TIMEOUT = millis() + MC_TIMEOUT_LIMIT;

      can_message::command_data received;
      memcpy(received.bytes, buf, 8); // copy buf to received.bytes
      uint16_t angular_velocity = received.shorts[1]; // bytes 2,3 as per spec
      float linear_velocity = 2 * MATH_PI * WHEEL_RADIUS * angular_velocity * (60/1000); // this is in kmph
      
      if (linear_velocity > 5.0)
        use_regen = true; // regen only if reported speed is above 5 kmph
      else
        use_regen = false;
    } 
    else if (CAN.getCanId() == AM::MAIN_MESSAGE_GET) { // shutdown command from main arduino or WHATEVER, NEED DATA ON THAT!*****************************************************************************
    MAIN_TIMEOUT = millis() + MAIN_TIMEOUT_LIMIT;
      if(buf[0] == 42) { // everything is fine
        run_normal = !total_shutdown; // I am using a not statement in case something wierd happens. Trust me.
      }
      
      else if(buf[0] == 25) { // a critical error that is unrecoverable has occurred
        total_shutdown = true;
        run_normal = false;
      }
      
      else if(buf[0] == 64) { // the main arduino requests a temporary stop
        run_normal = false;
      }
      else {
        run_normal = false;
      }
        
    
    }
  } // end of "if can message available"
  
  if(millis() > MAIN_TIMEOUT || millis() > MC_TIMEOUT) {
    run_normal = false;
  }
  
  if(DEBUG_ON > 0 && millis() > debugPrintTimer) {
    if(total_shutdown) {
      Serial.println("total shutdown");
    }
    if(run_normal) {
      Serial.println("run normal");
    }
    else {
      Serial.println("standby");
    }
    
    if(millis() > MAIN_TIMEOUT) {
      Serial.println("main timeout");
    }
    
    if(millis() > MC_TIMEOUT) {
      Serial.println("motor controller timeout");
    }
    
    
    debugPrintTimer += DEBUG_PRINT_DELAY;   
    
  } // end of "if debug is on"
}// end of "void loop"
