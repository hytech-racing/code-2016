


#include <SPI.h>
#include "mcp_can.h"
#include <stdint.h>
#include "config.h"
#include "maisc_functions.h"
#include "arduinoMain.h"
#include "mc.h"


//EV DC - Electric Vehicle Driver Controls

bool use_regen = false; // I know you hate global variables but I had to do this :'(
bool regen_button = false;
bool total_shutdown = false;
bool run_normal = false;
boolean inverter_enabled = false;
int launchParameter = 0;
int messagePrinterCounter = 0;

long MC_TIMEOUT = 0;
int torque_reduction = 0;
float DCbusCurrent = 0;
long MAIN_TIMEOUT = 0;
long MAIN_SENDER_TIMER = 0;
long MC_SENDER_TIMER = 0;
long debugPrintTimer = 0;
float accelFilter[5];
float accelFilterCoefs[3] = {1, 1, 1}; // in case we want to implement some sort of filter
float brakeFilter[5];
float brakeFilterCoefs[3] = {1, 1, 1};



MCP_CAN CAN(SPI_CS_PIN);

// function to send CAN data more easily

void send_can_message(int id, unsigned char *message) {
    CAN.sendMsgBuf(id, 0, 8, message);
}


















void setup() { // BEGIN SETUP~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 unsigned char len = 0;
 unsigned char buf[8];
 long setupTimer = 0; // all-purpose setup timer
 boolean good_for_launch = false;
 boolean inverter_enabled = false;
 boolean init_confirmed = false;

  
  Serial.begin(9600); //initialize serial for PC communication

  while (CAN.begin(CAN_RATE) != CAN_OK) { //Communication rate is 500Kbps
    Serial.println("CAN BUS Shield init FAIL");
    delay(100);
  }
  
  Serial.println("CAN BUS Shield Init OK");
  
  pinMode(TWELVE_ON_PIN, OUTPUT); // set pin modes
  pinMode(BRAKE_LIGHTS_PIN, OUTPUT);

  // here I set filters so that the can chips only interpret messages from four IDs.
  
  CAN.init_Filt(0, 0, MC_INT_STATES_MESSAGE_ID);
  CAN.init_Filt(1, 0, MC_MOTOR_POS_INFO_MESSAGE_ID);
  CAN.init_Filt(2, 0, AM::MAIN_MESSAGE_GET);
  CAN.init_Filt(3, 0, 0x0AB);
  CAN.init_Filt(4, 0, BMS_CURRENT_MESSAGE);
  
  digitalWrite(TWELVE_ON_PIN, HIGH); // turns on motor controller DCDC converters

  // while the motor controler is not enabled, wait for the main arduino to 
  // give the launch signal and wait for the motor controller to be enabled
  
  while(!inverter_enabled) { // while the motor controller is not enabled
  
    //get values from pedals
    
    float brake_input_voltage = get_input_voltage(BRAKE_PEDAL, 5.0); //get brake value for main arduino
    float accel_input_voltage_1 = get_input_voltage(ACCEL_PEDAL_1, 5.0);
    float accel_input_voltage_2 = get_input_voltage(ACCEL_PEDAL_2, 5.0); // two voltages needed to compare
    
    // print out unadjusted values
    
    if(DEBUG_ON > 0 && millis() > debugPrintTimer) {
      Serial.println("raw: brake, then acc_1, then acc_2");
      Serial.println(brake_input_voltage);
      Serial.println(accel_input_voltage_1);
      Serial.println(accel_input_voltage_2);
    }
    
    // map the values between the max and minumum pedal values
    
    accel_input_voltage_1 = mapFloat(accel_input_voltage_1, ACC1_NO_VAL, ACC1_ALL_VAL, 0, 5.0);
    accel_input_voltage_2 = mapFloat(accel_input_voltage_2, ACC2_NO_VAL, ACC2_ALL_VAL, 0, 5.0);
    brake_input_voltage = mapFloat(brake_input_voltage, BRAKE_NO_VAL, BRAKE_ALL_VAL, 0, 5.0);
    
    // the values shouldn't above 5 volts or below 0 volts
    
    if(accel_input_voltage_1 > 5.0) {
      accel_input_voltage_1 = 5.0;
    }
    else if(accel_input_voltage_1 < 0) {
      accel_input_voltage_1 = 0;
    }
    
    if(accel_input_voltage_2 > 5.0) {
      accel_input_voltage_2 = 5.0;
    }
    else if(accel_input_voltage_2 < 0) {
      accel_input_voltage_2 = 0;
    }
    
    if(brake_input_voltage > 5.0) {
      brake_input_voltage = 5.0;
    }
    else if(brake_input_voltage < 0){
      brake_input_voltage = 0;
    }
    
    if(brake_input_voltage > BRAKE_LIGHT_THRESHOLD) { // lols brake lights
      activate_brake_lights();
    }
    else{
      deactivate_brake_lights();
    }
    
    // print out mapped pedal values
    
    if(DEBUG_ON > 0 && millis() > debugPrintTimer) {
      Serial.println("mapped: brake, then acc_1, acc_2");
      Serial.println(brake_input_voltage);
      Serial.println(accel_input_voltage_1);
      Serial.println(accel_input_voltage_2);
      float diff = abs(accel_input_voltage_1 - accel_input_voltage_2);
      Serial.print(" difference: ");
      Serial.println(diff);
      Serial.println("not launched yet");
      Serial.print("launch parameter: ");
      Serial.println(launchParameter);
      debugPrintTimer += DEBUG_PRINT_DELAY;
    }
    
    setupTimer = millis() + 50;
    
    // here I send a disable message so that the motor controller does not time out
    
    generate_MC_message(buf,0,false,false);
    send_can_message(0x0C0, buf);
    
    // here I send a message to the main arduino giving it pedal values
  
    unsigned char message[8];
    for(int i = 0; i < 8; i++) {
      message[i] = 0;
    }
    message[1] = (unsigned char) (brake_input_voltage * 20.0); // brakes from 0 to 100
    message[3] = (unsigned char) (min(accel_input_voltage_1, accel_input_voltage_2)*20.0); // accelerator from 0 to 100
    send_can_message(AM::MAIN_MESSAGE_SEND, message);
    
    // here, I listen to CAN in order to determine if the main arduino
    // wants it to go 
       
    while(setupTimer > millis()){ //listen to can for 50 milisnencends
      if(CAN.checkReceive() == CAN_MSGAVAIL) { //Checks if message is available
        CAN.readMsgBuf(&len, buf); 
        if (CAN.getCanId() == AM::MAIN_MESSAGE_GET){ // init command from main arduino
          launchParameter = buf[0];
        }
        if(launchParameter == 42) { 
          init_confirmed = true; // confirms the ready to drive signal
            Serial.println("got launch messsage");
        }
        
      }
    }
    
    // now, I wait for the motor controller to respond saying that is is enabled.
    
    while (init_confirmed && !inverter_enabled) {// while main arduino initialization
                                                 // is confirmed and motor
                                                 // controller is not enabled  

      Serial.println("waiting for motor controller");
      
      // to undo the lockout on the motor controller, you
      // have to send one disable command amid a series of enable commands
      
      for(int i = 0; i < 10; i++) {
        generate_MC_message(buf,0,false,true); // many enable commands
        send_can_message(0x0C0, buf);
        delay(10);
      }
        
      generate_MC_message(buf,0,false,false);//this is the disable command
      send_can_message(0x0C0, buf);
      delay(40);
        
      for(int i = 0; i < 10; i++) {
        generate_MC_message(buf,0,false,true); // more enable commands
        send_can_message(0x0C0, buf);
        delay(10);
      }
      

      
      float brake_input_voltage = get_input_voltage(BRAKE_PEDAL, 5.0); //get brake value for main arduino
      float accel_input_voltage_1 = get_input_voltage(ACCEL_PEDAL_1, 5.0);
      float accel_input_voltage_2 = get_input_voltage(ACCEL_PEDAL_2, 5.0); // two voltages needed to compare
    
      // print raw pedal values
    
      if(DEBUG_ON > 0 && millis() > debugPrintTimer) {
        Serial.println("raw: brake, then acc_1, then acc_2");
        Serial.println(brake_input_voltage);
        Serial.println(accel_input_voltage_1);
        Serial.println(accel_input_voltage_2);
      
      }
    
      accel_input_voltage_1 = mapFloat(accel_input_voltage_1, ACC1_NO_VAL, ACC1_ALL_VAL, 0, 5.0);
      accel_input_voltage_2 = mapFloat(accel_input_voltage_2, ACC2_NO_VAL, ACC2_ALL_VAL, 0, 5.0);
      brake_input_voltage = mapFloat(brake_input_voltage, BRAKE_NO_VAL, BRAKE_ALL_VAL, 0, 5.0);
    
      // print mapped values
    
      if(DEBUG_ON > 0 && millis() > debugPrintTimer) {
        Serial.println("mapped: brake, then acc_1, acc_2");
        Serial.println(brake_input_voltage);
        Serial.println(accel_input_voltage_1);
        Serial.println(accel_input_voltage_2);
        float diff = abs(accel_input_voltage_1 - accel_input_voltage_2);
        Serial.print(" difference: ");
        Serial.println(diff);
        Serial.print("launch parameter: ");
        Serial.println(launchParameter);
        debugPrintTimer += DEBUG_PRINT_DELAY;
      }
    
      // adjust brake values between 0 and 5 volts
    
      if(accel_input_voltage_1 > 5.0) {
        accel_input_voltage_1 = 5.0;
      }
      else if(accel_input_voltage_1 < 0) {
        accel_input_voltage_1 = 0;
      }
    
      if(accel_input_voltage_2 > 5.0) {
        accel_input_voltage_2 = 5.0;
      }
      else if(accel_input_voltage_2 < 0) {
        accel_input_voltage_2 = 0;
      }
    
      if(brake_input_voltage > 5.0) {
        brake_input_voltage = 5.0;
      }
      else if(brake_input_voltage < 0){
        brake_input_voltage = 0;
      }
    
      if(brake_input_voltage > BRAKE_LIGHT_THRESHOLD) {// lols brake lights
        activate_brake_lights();
      }
      else{
        deactivate_brake_lights();
      }
            
      unsigned char messageOther[8]; // send brakes and stuff to main arduino
      for(int i = 0; i < 8; i++) {
        messageOther[i] = 0;
      } 
      messageOther[1] = (unsigned char) (brake_input_voltage * 20.0); // brakes from 0 to 100
      messageOther[3] = (unsigned char) (min(accel_input_voltage_1, accel_input_voltage_2)*20.0); // accelerator from 0 to 100

      send_can_message(AM::MAIN_MESSAGE_SEND, messageOther);
      
      
      // here I listen to CAN for enabled signals
      // from the motor controller
      
      setupTimer = millis() + 50;
      
      while(setupTimer > millis()){ //listen to can for 50 millisecdons
        if(CAN.checkReceive() == CAN_MSGAVAIL) {
          CAN.readMsgBuf(&len, buf);
          
          if (CAN.getCanId() == MC_INT_STATES_MESSAGE_ID){ // if the message has 
                                                           // motor controller
                                                           // state data
            if(buf[6] == 1) {
              inverter_enabled = true; // bit 6 of byte 6 is the inverter state
              good_for_launch = true;
              Serial.println("got good to go message");
            }
            for(int i = 0; i < 8; i++) { // print out the motor controller message
              Serial.print(buf[i], HEX);
              Serial.write(32);
            }
            Serial.println("  ");
          }
          else if (CAN.getCanId() == AM::MAIN_MESSAGE_GET){ // init command from main arduino
            launchParameter = buf[0];
            Serial.print("launch parameter ");
            Serial.println(launchParameter);
            if(buf[0] == 42){
              init_confirmed = true;
            }
          }
        }
      }
    } // end of "while inverter not enabled and init confirmed"
  }// end of "while not good for launch"
}// end of "void setup"






























































void loop() { 
  
  // the loop is pretty basic- get any CAN messages I need, get pedal values, process them, and then send commands
  // to the motor controller
  
  unsigned char len = 0; 
  unsigned char buf[8];  
  unsigned char mainMessage[8];
  unsigned char MCmessage[8];
  boolean pedal_error = false;
  for(int i = 0; i < 8; i++) {
    mainMessage[i] = 0;
  }
  
  long CANtimer = millis() + 20; // listen to CAN for 20 ms
  while(CANtimer > millis()) { 
  if(CAN.checkReceive() == CAN_MSGAVAIL) {
    CAN.readMsgBuf(&len, buf); 
    if (CAN.getCanId() == MC_MOTOR_POS_INFO_MESSAGE_ID) {// if it is a motor position
                                                         // message, calculate
                                                         // ground speed
      MC_TIMEOUT = millis() + MC_TIMEOUT_LIMIT;
      float angular_velocity = MC::getRPM(buf);
      float linear_velocity = 2 * MATH_PI * WHEEL_RADIUS * angular_velocity * (60/1000)* (TEETH_MOTOR / TEETH_WHEEL); // this is in kmph
       
      if (linear_velocity > 5.0) {
        use_regen = regen_button; // regen only if reported speed is above 5 kmph
      }
      else {
        use_regen = false;
      }
    } 
    else if(CAN.getCanId() == MC_INT_STATES_MESSAGE_ID) { // print out controller states message
      Serial.print("motor mesage ");
      for(int i = 0; i < 8; i++) {
        Serial.print(buf[i]);
        Serial.print(" ");
      }
      Serial.println(" ");
    }
    else if(CAN.getCanId() == BMS_CURRENT_MESSAGE) { // load DC current into variable
      DCbusCurrent = getBMScurrent(buf);
    }
    else if(CAN.getCanId() == 0x0AB) { // print out controller error messages
     Serial.print("ERROR mesage ");
     for(int i = 0; i < 8; i++) {
        Serial.print(buf[i]);
        Serial.print(" ");
      }
      Serial.println(" ");
    }
    else if (CAN.getCanId() == AM::MAIN_MESSAGE_GET) { // get shutdown message
                                                       // and regen value from main
                                                       // arduino
      MAIN_TIMEOUT = millis() + MAIN_TIMEOUT_LIMIT;
      if(buf[0] == 42) { // everything is fine
        run_normal = true; // 
      }
      else { // something is wrong
        run_normal = false;
      }
        
      regen_button = AM::getRegenButton(buf);
    
      }
    } // end of "if can message available"
  }// end of "while can timer is on"
  
  // get pedal values
  
  float brake_input_voltage = get_input_voltage(BRAKE_PEDAL, 5.0);
  float accel_input_voltage_1 = get_input_voltage(ACCEL_PEDAL_1, 5.0);
  float accel_input_voltage_2 = get_input_voltage(ACCEL_PEDAL_2, 5.0); // two voltages needed to compare
       
  // if one of the pedal wires is broken, then it will read 5v or 0 volts.
  // so, if you see one of those errors, set a flag
       
  if(brake_input_voltage > 4.75 || brake_input_voltage < 0.10) {                                         
    pedal_error = true;
  }
  
  if(max(accel_input_voltage_1, accel_input_voltage_2) > 4.75 || min(accel_input_voltage_1, accel_input_voltage_2) < 0.10){
    pedal_error = true;
  }
  
  // send raw pedal values
  
  if(DEBUG_ON > 0 && millis() > debugPrintTimer) {
    
    Serial.println("raw: brake, then acc_1, then acc_2");
    Serial.println(brake_input_voltage);
    Serial.println(accel_input_voltage_1);
    Serial.println(accel_input_voltage_2);
  }
  
  // map the pedal values and restrict them to be between 0 and 5 volts
        
  brake_input_voltage = mapFloat(brake_input_voltage, BRAKE_NO_VAL, BRAKE_ALL_VAL, 0, 5.0); // map recieved voltages to true voltages
  accel_input_voltage_1 = mapFloat(accel_input_voltage_1, ACC1_NO_VAL, ACC1_ALL_VAL, 0, 5.0);
  accel_input_voltage_2 = mapFloat(accel_input_voltage_2, ACC2_NO_VAL, ACC2_ALL_VAL, 0, 5.0);
  
  if(accel_input_voltage_1 < 0) {
    accel_input_voltage_1 = 0;
  }
  if(accel_input_voltage_2 < 0) {
    accel_input_voltage_2 = 0;
  }
  if(brake_input_voltage < 0) {
    brake_input_voltage = 0;
  }
  
  if(accel_input_voltage_1 > 5.0) {
    accel_input_voltage_1 = 5.0;
  }
  if(accel_input_voltage_2 > 5.0) {
    accel_input_voltage_2 = 5.0;
  }
  if(brake_input_voltage > 5.0) {
    brake_input_voltage = 5.0;
  }
  
  // print the mapped pedal values
  
  if(DEBUG_ON > 0 && millis() > debugPrintTimer) {
    Serial.println("mapped: brake, then acc_1, acc_2");
    Serial.println(brake_input_voltage);
    Serial.println(accel_input_voltage_1);
    Serial.println(accel_input_voltage_2);
    float diff = abs(accel_input_voltage_1 - accel_input_voltage_2);
    Serial.print(" difference: ");
    Serial.println(diff);
  }
  
  if(brake_input_voltage > BRAKE_LIGHT_THRESHOLD) { // lols brake lights
    activate_brake_lights();
  }
  else {
    deactivate_brake_lights();
  }
  
  if (abs(accel_input_voltage_1 - accel_input_voltage_2) > MAX_VOLTAGE_DIFFERENCE) {
    
    // if the pedals disagree too much, throw an error
    
    pedal_error = true;
  }
  
  float operating_accel_1 = accel_input_voltage_1; // these values are set to 0 if there is an error
  float operating_accel_2 = accel_input_voltage_2; // so that the pedal values before errors can still be sent to
                                                   // the main arduino
  
  if(!run_normal) { // set pedal signals to be 0 if there is a stand-by condition
    operating_accel_1 = 0;
    operating_accel_2 = 0; 
  }
  else {
    mainMessage[0] = 42; // if no problem, tell main arduino that EVDC is running normal
  }
  
  if(pedal_error){ // if there is a pedal issue, then set the pedal signals to 0.
                   // also tell main ardunio that there is a pedal error
    operating_accel_1 = 0;
    operating_accel_2 = 0;
    mainMessage[2] = 1;
    Serial.println("pedal error");
  }
  
  mainMessage[1] = (unsigned char) (brake_input_voltage * 20.0); // brakes from 0 to 100
  mainMessage[3] = (unsigned char) ((accel_input_voltage_1 + accel_input_voltage_2)*10.0); // accelerator from 0 to 100 CHANGE BACK TO MIN FOR ACTUAL PROGRAM
  
  if(millis() > MAIN_SENDER_TIMER){
    send_can_message(AM::MAIN_MESSAGE_SEND, mainMessage);
    MAIN_SENDER_TIMER += MAIN_SENDER_DELAY;
  }
  
  // here I implement a 3-point running averager in order
  // to filter the pedal values a bit
  
  float nextAccelerator = min(operating_accel_1, operating_accel_2) + accelFilter[0] + accelFilter[1];
  nextAccelerator /= 3;
  accelFilter[1] = accelFilter[0];
  accelFilter[0] = min(operating_accel_1, operating_accel_2);
  
 // here I conpute the torque value for the next command message
  
  int nextTorque;
  nextTorque = compute_torque(nextAccelerator, brake_input_voltage, use_regen);
  
  // the motor controller does not have a DC bus current limit, so I have to implement a limit myself
  // what I do here is, if he current is above 160 amps, the torque is repeatedly reduced. If the current
  // is less than 160, the reduction is repeatedly taken away
  
  if(DCbusCurrent > 160) {
    int error = DCbusCurrent - 160; 
    torque_reduction += error*2; // reduce by .2 nm for every amp above 160 amps every loop (so ~50 times per second)
  }
  else if(torque_reduction > 0) {
    int error = 160-DCbusCurrent;
    torque_reduction -= error*4; // remove reduction if current is less than 160 amps. The removal is faster than the reduction
  }
  
  if(torque_reduction < 0) {
    torque_reduction = 0;
  }
  
  nextTorque -= torque_reduction;
    
 // here I make and send the motor controller message. the parameters are
 // a char pointer to load, acc pedal value, brake pedal value, regen, move in reverse, enable  
    
  if(millis() > MC_SENDER_TIMER){   
    generate_MC_message(MCmessage, nextTorque, false, run_normal);
    send_can_message(MAIN_COMMAND_MESSAGE_ID, MCmessage);
    MC_SENDER_TIMER += MC_SENDER_DELAY;
  }
  
  //  here I listen to CAN to get motor speed, motor states, print out errors, 
  //  and check for disable messages from the main arduino
  
  
  
  if(millis() > MAIN_TIMEOUT || millis() > MC_TIMEOUT) { // stop operation if the main
                                                         // or motor controller time out
    run_normal = false;
  }
  
  if((DEBUG_ON > 0) && (millis() > debugPrintTimer)) { // occasional debug print
    
    Serial.print("pedals filtered: ");
    Serial.println(nextAccelerator);
    
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
