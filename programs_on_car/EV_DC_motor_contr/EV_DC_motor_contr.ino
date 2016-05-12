


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
bool regen_button = false;
bool total_shutdown = false;
bool run_normal = false;
boolean inverter_enabled = false;
int launchParameter;
int messagePrinterCounter = 0;

long MC_TIMEOUT = 0;
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
  //MCP_CAN CAN(SPI_CS_PIN);
  unsigned char len = 0;
  unsigned char buf[8];
 long setupTimer = 0; // all-purpose setup timer

  
  Serial.begin(9600); //initialize serial for PC communication

  /*
    Initializes CAN controller
  */
  while (CAN.begin(CAN_RATE) != CAN_OK) { //Communication rate is 500Kbps
    Serial.println("CAN BUS Shield init FAIL");
    delay(100);
  }
  
  Serial.println("CAN BUS Shield Init OK");
  
  pinMode(TWELVE_ON_PIN, OUTPUT); // set pins to the correst mode
  pinMode(BRAKE_LIGHTS_PIN, OUTPUT);

  // here I set filters so that the can chips only interpret messages from four IDs.
  
  CAN.init_Filt(0, 0, MC_INT_STATES_MESSAGE_ID);
  CAN.init_Filt(1, 0, MC_MOTOR_POS_INFO_MESSAGE_ID);
  CAN.init_Filt(2, 0, AM::MAIN_MESSAGE_GET);
  CAN.init_Filt(3, 0, 0x0AB);

  
  boolean good_for_launch = false;
  boolean inverter_enabled = false;
  boolean init_confirmed = false;

  
  while(!good_for_launch) { // PRIMARY WHILE  PRIMARY WHILE  PRIMARY WHILE  PRIMARY WHILE  PRIMARY WHILE  PRIMARY WHILE  PRIMARY WHILE  PRIMARY WHILE  PRIMARY WHILE  PRIMARY WHILE  PRIMARY WHILE
    
    // get values from pedals
    
    float brake_input_voltage = get_input_voltage(BRAKE_PEDAL, 5.0);
    float accel_input_voltage_1 = get_input_voltage(ACCEL_PEDAL_1, 5.0);
    float accel_input_voltage_2 = get_input_voltage(ACCEL_PEDAL_2, 5.0);
    
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
    
    // the values shouldn't be 
    
    if(brake_input_voltage > 5) {
      brake_input_voltage = 5;
    }
    else if(brake_input_voltage < 0){
      brake_input_voltage = 0;
    }
    
    if(brake_input_voltage > BRAKE_LIGHT_THRESHOLD) {
      activate_brake_lights();
    }
    else{
      deactivate_brake_lights();
    }
    
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
    while(setupTimer > millis()){ //listen to can for 50 milisnencends
      if(CAN.checkReceive() == CAN_MSGAVAIL) { //Checks if message is available
        CAN.readMsgBuf(&len, buf); //If so, stores length & value
        // You have to readMsgBuf before you can get the ID of the message
      }
        else if (CAN.getCanId() == AM::MAIN_MESSAGE_GET){ // init command from main arduino
          launchParameter = buf[0];
          if(launchParameter == 42) {
            init_confirmed = true; // or WHATEVER, NEED DATA ON THAT!**
            Serial.println("got launch messsage");
          }
        }
      }
    
    digitalWrite(TWELVE_ON_PIN, HIGH); // turns on motor controller DCDC converters
    //send_can_message(command_message(0, 0, false, false, false, 0), 0x0C0); // Send at least one message to prevent MC shutdown
    generate_MC_message(buf,0,brake_input_voltage,false,true,false);
    send_can_message(0x0C0, buf);
    
  
    unsigned char message[8];
    for(int i = 0; i < 8; i++) {
      message[i] = 0;
    }
    message[1] = char(brake_input_voltage * 20.0); // brakes from 0 to 100
    message[3] = char((accel_input_voltage_1 + accel_input_voltage_2)*10.0); // accelerator from 0 to 100
    send_can_message(AM::MAIN_MESSAGE_SEND, message);
       
    
    
    // wait for inverter to turn on
    while (init_confirmed && !inverter_enabled) {// SECONDARY WHILE  SECONDARY WHILE  SECONDARY WHILE  SECONDARY WHILE  SECONDARY WHILE  SECONDARY WHILE  SECONDARY WHILE  SECONDARY WHILE  SECONDARY WHILE  
      // while it is not enabled but main arduino initialization is finished, send out enable commands, then a single disable
      
      Serial.println("waiting for motor controller");
      
      
      for(int i = 0; i < 10; i++) {
        generate_MC_message(buf,0,brake_input_voltage,false,false,true);
        send_can_message(0x0C0, buf);
        delay(10);
      }
        
      generate_MC_message(buf,0,brake_input_voltage,false,false,false);//this is the disable!
      send_can_message(0x0C0, buf);
      delay(40);
        
      for(int i = 0; i < 10; i++) {
        generate_MC_message(buf,0,brake_input_voltage,false,false,true);
        send_can_message(0x0C0, buf);
        delay(10);
      }
      

      
    float brake_input_voltage = get_input_voltage(BRAKE_PEDAL, 5.0); //get brake value for main arduino
    float accel_input_voltage_1 = get_input_voltage(ACCEL_PEDAL_1, 5.0),
            accel_input_voltage_2 = get_input_voltage(ACCEL_PEDAL_2, 5.0); // two voltages needed to compare
    if(DEBUG_ON > 0 && millis() > debugPrintTimer) {
      Serial.println("raw: brake, then acc_1, then acc_2");
      Serial.println(brake_input_voltage);
      Serial.println(accel_input_voltage_1);
      Serial.println(accel_input_voltage_2);
      
    }
    
    accel_input_voltage_1 = mapFloat(accel_input_voltage_1, ACC1_NO_VAL, ACC1_ALL_VAL, 0, 5.0);
    accel_input_voltage_2 = mapFloat(accel_input_voltage_2, ACC2_NO_VAL, ACC2_ALL_VAL, 0, 5.0);
    brake_input_voltage = mapFloat(brake_input_voltage, BRAKE_NO_VAL, BRAKE_ALL_VAL, 0, 5.0);
    
    
    
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
    
    if(brake_input_voltage > 5) {
      brake_input_voltage = 5;
    }
    else if(brake_input_voltage < 0){
      brake_input_voltage = 0;
    }
    
    if(brake_input_voltage > BRAKE_LIGHT_THRESHOLD) {
      activate_brake_lights();
    }
    else{
      deactivate_brake_lights();
    }
    
            
      unsigned char messageOther[8];
      for(int i = 0; i < 8; i++) {
        messageOther[i] = 0;
      } 
      messageOther[1] = int(brake_input_voltage * 20.0); // brakes from 0 to 100
      messageOther[3] = char((accel_input_voltage_1 + accel_input_voltage_2)*10.0); // accelerator from 0 to 100

      send_can_message(AM::MAIN_MESSAGE_SEND, messageOther);
      
      unsigned char len = 0; //Length of received message
      //unsigned char buf[8];  //Where the received message will be stored
      
      setupTimer = millis() + 50;
      while(setupTimer > millis()){ //listen to can for 50 millisecdons
        if(CAN.checkReceive() == CAN_MSGAVAIL) { //Checks if message is available
          CAN.readMsgBuf(&len, buf); //If so, stores length & value
          // You have to readMsgBuf before you can get the ID of the message
          
          if (CAN.getCanId() == MC_INT_STATES_MESSAGE_ID){ // this has the inverter data
            if(buf[6] == 1) {
              inverter_enabled = true; // bit 0 of byte 5 as per specs is the inverter state
              good_for_launch = true;
              Serial.println("got good to go message");
            }
            for(int i = 0; i < 8; i++) {
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
      if(init_confirmed && !inverter_enabled) {
        Serial.println("not continuing");
      }
    } // end of "while inverter not enabled and init confirmed"
  }// end of "while not good for launch"
}// end of "void setup"






























































void loop() { // BEGIN LOOP~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  
  unsigned char len = 0; //Length of received message
  unsigned char buf[8];  //Where the received message will be stored
  unsigned char mainMessage[8];
  unsigned char MCmessage[8];
  for(int i = 0; i < 8; i++) {
    mainMessage[i] = 0;
  }
  boolean pedal_error = false;
  
  
  float brake_input_voltage = get_input_voltage(BRAKE_PEDAL, 5.0),
       accel_input_voltage_1 = get_input_voltage(ACCEL_PEDAL_1, 5.0),
       accel_input_voltage_2 = get_input_voltage(ACCEL_PEDAL_2, 5.0); // two voltages needed to compare
       
  if(brake_input_voltage > 4.75 || brake_input_voltage < 0.25) {
    pedal_error = true;
  }
  
  if(max(accel_input_voltage_1, accel_input_voltage_2) > 4.75 || min(accel_input_voltage_1, accel_input_voltage_2) < 0.25){
    pedal_error = true;
  }
  
  
  if(DEBUG_ON > 0 && millis() > debugPrintTimer) {
    //activate_brake_lights();
    
    Serial.println("raw: brake, then acc_1, then acc_2");
    Serial.println(brake_input_voltage);
    Serial.println(accel_input_voltage_1);
    Serial.println(accel_input_voltage_2);
  }
  
        
  brake_input_voltage = mapFloat(brake_input_voltage, BRAKE_NO_VAL, BRAKE_ALL_VAL, 0, 5.0); // map recieved voltages to true voltages
  accel_input_voltage_1 = mapFloat(accel_input_voltage_1, ACC1_NO_VAL, ACC1_ALL_VAL, 0, 5.0);
  accel_input_voltage_2 = mapFloat(accel_input_voltage_2, ACC2_NO_VAL, ACC2_ALL_VAL, 0, 5.0);
  
  
  
  if(DEBUG_ON > 0 && millis() > debugPrintTimer) {
      Serial.println("mapped: brake, then acc_1, acc_2");
      Serial.println(brake_input_voltage);
      Serial.println(accel_input_voltage_1);
      Serial.println(accel_input_voltage_2);
      float diff = abs(accel_input_voltage_1 - accel_input_voltage_2);
      Serial.print(" difference: ");
      Serial.println(diff);
    }
  
  
  
  
  
  if (abs(accel_input_voltage_1 - accel_input_voltage_2) > MAX_VOLTAGE_DIFFERENCE) {
      pedal_error = true;
  }
  
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
    
  if(!run_normal) {   
    accel_input_voltage_1 = 0;
    accel_input_voltage_2 = 0; 
  }
  else {
    mainMessage[0] = 42; // EVDC is running normal
  }

  if(brake_input_voltage > BRAKE_LIGHT_THRESHOLD) {
      activate_brake_lights();
    }
    else{
      deactivate_brake_lights();
    }
    
  
  if(pedal_error){
    accel_input_voltage_1 = 0;// and kill the car, because wtf
    accel_input_voltage_2 = 0;
  }
  
  
  
    
 
  
  
  if(pedal_error) {
    mainMessage[2] = 1;
    Serial.println("pedal error");
  }
  mainMessage[1] = char(brake_input_voltage * 20.0); // brakes from 0 to 100
  mainMessage[3] = char(min(accel_input_voltage_1, accel_input_voltage_2)*20.0);
  
  
  if(millis() > MAIN_SENDER_TIMER){
    send_can_message(AM::MAIN_MESSAGE_SEND, mainMessage);
    MAIN_SENDER_TIMER += MAIN_SENDER_DELAY;
  }
  
  float nextAccelerator = min(accel_input_voltage_1, accel_input_voltage_2) + accelFilter[0] + accelFilter[1];
  nextAccelerator /= 3;
  accelFilter[1] = accelFilter[0];
  accelFilter[0] = min(accel_input_voltage_1, accel_input_voltage_2);
    
 //   MAKE MOTOR CONTROLER MESSAGE   MAKE MOTOR CONTROLER MESSAGE   MAKE MOTOR CONTROLER MESSAGE   MAKE MOTOR CONTROLER MESSAGE  MAKE MOTOR CONTROLER MESSAGE  MAKE MOTOR CONTROLER MESSAGE
    
   if(millis() > MC_SENDER_TIMER){
     
    generate_MC_message(MCmessage, nextAccelerator, brake_input_voltage, use_regen, false, run_normal);
    send_can_message(MAIN_COMMAND_MESSAGE_ID, MCmessage);
    MC_SENDER_TIMER += MC_SENDER_DELAY;
  }
  
  
  long CANtimer = millis() + 20;
  while(CANtimer > millis()) {
      
    if(CAN.checkReceive() == CAN_MSGAVAIL) { //Checks if message is available
      CAN.readMsgBuf(&len, buf); //If so, stores length & value
      
      if (CAN.getCanId() == MC_MOTOR_POS_INFO_MESSAGE_ID) {
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
      else if(CAN.getCanId() == MC_INT_STATES_MESSAGE_ID) {
        Serial.print("motor mesage ");
        for(int i = 0; i < 8; i++) {
          Serial.print(buf[i]);
          Serial.print(" ");
        }
        Serial.println(" ");
      }
      else if(CAN.getCanId() == 0x0AB) {
       Serial.print("ERROR mesage ");
       for(int i = 0; i < 8; i++) {
          Serial.print(buf[i]);
          Serial.print(" ");
        }
        Serial.println(" ");
      }
      else if (CAN.getCanId() == AM::MAIN_MESSAGE_GET) { // shutdown command from main arduino or WHATEVER, NEED DATA ON THAT!*****************************************************************************
      MAIN_TIMEOUT = millis() + MAIN_TIMEOUT_LIMIT;
        if(buf[0] == 42) { // everything is fine
          run_normal = true; // 
        }
        
        else if(buf[0] == 25) { // a critical error that is unrecoverable has occurred
          run_normal = false;
        }
        
        else if(buf[0] == 64) { // the main arduino requests a temporary stop
          run_normal = false;
        }
        else {
          run_normal = false;
        }
        
        regen_button = AM::getRegenButton(buf);
      
      }
    } // end of "if can message available"
  }// end of "while can timer is on"
  if(millis() > MAIN_TIMEOUT || millis() > MC_TIMEOUT) {
    run_normal = false;
  }
  
  if(DEBUG_ON > 0 && millis() > debugPrintTimer) {
    
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
