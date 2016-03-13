void BMSlightOn() {
  digitalWrite(BMS_LED, HIGH);
}

void BMSlightOff() {
  digitalWrite(BMS_LED, LOW);
}

void IMDlightOn() {
  digitalWrite(IMD_LED, HIGH);
}

void IMDlightOff() {
  digitalWrite(IMD_LED, LOW);
}

boolean shutdownButton() {
  return getMultiplexerDigital(SHUTDOWN_BUTTON_SELECT);
}

boolean boostButton() {
  return getMultiplexerDigital(BOOST_BUTTON_SELECT);
}

boolean cycleButton() {
  return getMultiplexerDigital(CYCLE_BUTTON_SELECT);
}

boolean toggleButton() {
  return getMultiplexerDigital(TOGGLE_BUTTON_SELECT);
}



