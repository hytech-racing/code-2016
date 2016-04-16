// it's misspelled so that it appears alphabetically in the row of tabs

/* list of functions:

activate_brake_lights
deactivate_brake_lights
get_input_voltage

mapFloat

*/


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
