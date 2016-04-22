// it's misspelled so that it appears alphabetically in the row of tabs

/* list of functions:

activate_brake_lights
deactivate_brake_lights
get_input_voltage

mapFloat

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

int compute_torque(float input_voltage, float brake_voltage, boolean regen) {
    float output_current;
    float unit_input_voltage = input_voltage/5; // so that unit_input_voltage goes from 0 to 1
    int divisor = 1;

//#define CALCULATE_DIVISOR for (divisor = 1;\
//    input_voltage > 0.99; input_voltage /= 10, divisor *= 10);

#if defined(CALCULATE_CURRENT_QUADRATIC) // x(2-x)
    //CALCULATE_DIVISOR
    output_current = divisor * unit_input_voltage * (2 - unit_input_voltage);
#elif defined(CALCULATE_CURRENT_CUBIC) // x^2(3-2x)
    //CALCULATE_DIVISOR
    output_current = divisor * input_voltage * input_voltage
        * (3 - 2 * input_voltage);
#elif defined(CALCULATE_CURRENT_CUBIC_TIMES_SQRT) // x^2.5(3-2x)
    //CALCULATE_DIVISOR
    output_current
        = divisor * unit_input_voltage * sqrt(unit_input_voltage)
        * (3 - 2 * unit_input_voltage);
#elif defined(CALCULATE_CURRENT_QUARTIC) // x^3(2.5-1.5x)
    //CALCULATE_DIVISOR
    output_current
        = divisor * unit_input_voltage * unit_input_voltage * unit_input_voltage
        * (2.5 - 1.5 * unit_input_voltage);
#elif defined(CALCULATE_CURRENT_SQRT_QUARTIC) // x^1.5(2.5-1.5x)
   // CALCULATE_DIVISOR
    output_current
        = divisor * sqrt(unit_input_voltage * unit_input_voltage * unit_input_voltage)
        * (2.5 - 1.5 * unit_input_voltage);
#elif defined(CALCULATE_CURRENT_QUINTIC) // x^4(2-x)
   // CALCULATE_DIVISOR
    output_current
        = divisor * unit_input_voltage * unit_input_voltage
        * unit_input_voltage * unit_input_voltage
        * (2 - unit_input_voltage);
#else // x
    output_current = unit_input_voltage;
#endif

    output_current *= 100; // we have a max of 160 amps, but let's start at 100 for now
    // torque = amps * torque per amp
    float output_torque = output_current * current_coefficient;
    if(brake_voltage > 0.5) { // brakes on more than 10%
      output_torque = 0 - (brake_voltage/5.0)*80.0; // 80 Nm of braking? sounds like a plan
    }
    
    output_torque *= 10; // because of the MC protocol
    
    

    return int(output_torque);



}

void generate_MC_message(unsigned char* message, float input_voltage, float brake_voltage, boolean regen, boolean forwards, boolean enable) {
  int thatTorque = compute_torque(input_voltage, brake_voltage, regen);
  message[0] = thatTorque & 0xFF;
  message[1] = thatTorque>>(8);
  message[2] = 0;
  message[3] = 0;
  message[4] = char(forwards);
  message[5] = char(enable);
  message[6] = 0;
  message[7] = 0;
}
  
  
