// it's misspelled so that it appears alphabetically in the row of tabs

/* list of functions:

activate_brake_lights
deactivate_brake_lights
get_input_voltage

mapFloat
filterPedals

*/
const float current_coefficient = 0.54f;

void activate_brake_lights() {
  digitalWrite(BRAKE_LIGHTS_PIN, HIGH); //-- placeholder */
}

void deactivate_brake_lights() {
  digitalWrite(BRAKE_LIGHTS_PIN, LOW);// -- placeholder */
}

float get_input_voltage(int pin, float connected_voltage) {
    return analogRead(pin) * (connected_voltage / 1023.0);
}


float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

float filterPedals(float acc1, float acc2, float* oldAccs, float* accCoefs) {
  float nextAcc = min(acc1, acc2)*accCoefs[0];
  nextAcc += oldAccs[0]*accCoefs[1];
  nextAcc += oldAccs[1]*accCoefs[2];
  nextAcc /= accCoefs[0]+accCoefs[1]+accCoefs[2];
  oldAccs[1] = oldAccs[0];
  oldAccs[0] = min(acc1, acc2);
  return nextAcc;
}
  
  
  
  
  

int compute_torque(float input_voltage, float brake_voltage, boolean regen) {
    float output_current;
    float unit_input_voltage = mapFloat(input_voltage, 0.5, 5.0, 0.0, 1.0); // so that unit_input_voltage goes from 0 to 1, starting when the pedal is 10% pressed
    if(unit_input_voltage < 0) {
      unit_input_voltage = 0;
    }
   
    unit_input_voltage = -1.0 * unit_input_voltage * unit_input_voltage + 2*unit_input_voltage; // y = -x^2 + 2x
    Serial.print("unit_input after curve: ");
    Serial.println(unit_input_voltage);
    

    float output_torque = unit_input_voltage * 165; // our limit is 160, but might as well go overboard so that we know we can get max torque.
                                                    // the motor controller will limit the torque
    
    if(brake_voltage > BRAKE_LIGHT_THRESHOLD) { // brakes on
      if(regen){
        output_torque = 0- mapFloat(brake_voltage, BRAKE_LIGHT_THRESHOLD, 5.0, 0.0, 1.0)*20; // 20 Nm of braking? sounds like a plan
      }
      else{
        output_torque = 0;
      }
    }
    
    output_torque *= 10; // because of the MC protocol
    
    return int(output_torque);
}

void generate_MC_message(unsigned char* message, int thatTorque, boolean backwards, boolean enable) {
  Serial.print("torque: ");
  Serial.println(thatTorque);
  message[0] = thatTorque & 0xFF;
  message[1] = thatTorque>>(8);
  message[2] = 0;
  message[3] = 0;
  message[4] = char(backwards);
  message[5] = char(enable);
  message[6] = 0;
  message[7] = 0;
}

float getBMScurrent(unsigned char* data) {
    unsigned char high = data[2];
    unsigned char low = data[3];

    float current = (high << 8) | low;
    return current / 10; // units are 0.1A
}
  
  // other torque curves ot use
  
  // int divisor = 1;

//#define CALCULATE_DIVISOR for (divisor = 1;\
//    input_voltage > 0.99; input_voltage /= 10, divisor *= 10);
//
//#if defined(CALCULATE_CURRENT_QUADRATIC) // x(2-x)
//    //CALCULATE_DIVISOR
//    output_current = divisor * unit_input_voltage * (2 - unit_input_voltage);
//#elif defined(CALCULATE_CURRENT_CUBIC) // x^2(3-2x)
//    //CALCULATE_DIVISOR
//    output_current = divisor * unit_input_voltage * input_voltage
//        * (3 - 2 * input_voltage);
//#elif defined(CALCULATE_CURRENT_CUBIC_TIMES_SQRT) // x^2.5(3-2x)
//    //CALCULATE_DIVISOR
//    output_current
//        = divisor * unit_input_voltage * sqrt(unit_input_voltage)
//        * (3 - 2 * unit_input_voltage);
//#elif defined(CALCULATE_CURRENT_QUARTIC) // x^3(2.5-1.5x)
//    //CALCULATE_DIVISOR
//    output_current
//        = divisor * unit_input_voltage * unit_input_voltage * unit_input_voltage
//        * (2.5 - 1.5 * unit_input_voltage);
//#elif defined(CALCULATE_CURRENT_SQRT_QUARTIC) // x^1.5(2.5-1.5x)
//   // CALCULATE_DIVISOR
//    output_current
//        = divisor * sqrt(unit_input_voltage * unit_input_voltage * unit_input_voltage)
//        * (2.5 - 1.5 * unit_input_voltage);
//#elif defined(CALCULATE_CURRENT_QUINTIC) // x^4(2-x)
//   // CALCULATE_DIVISOR
//    output_current
//        = divisor * unit_input_voltage * unit_input_voltage
//        * unit_input_voltage * unit_input_voltage
//        * (2 - unit_input_voltage);
//#else // x
  
