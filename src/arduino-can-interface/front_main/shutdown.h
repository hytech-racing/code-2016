

void shutDownNormal(MCP_CAN& datCAN) {
  Serial.println("shutting down normal");
  for(int i = 0; i < 10; i++) { // wait a second to shut everything down
    EVDC::shutThemAllDown(datCAN);
    RPi::giveProgression(datCAN, 8);
    delay(100);
  }
  digitalWrite(AIRdcdc, LOW);
  digitalWrite(software_shutdown_control, LOW);
  while(1) {
      EVDC::shutThemAllDown(datCAN);
      RPi::giveProgression(datCAN, 8);
      delay(200);
  }

}

void shutdownError(MCP_CAN& datCAN, int code) {
  RPi::giveError(datCAN, code);
  
  if(code == 1 || code == 6 || code == 7 || code == 9 || code == 10) {    // 1, 6,7, 9, 10 are BMS errors and write to EEPROM
    EEPROM.write(0, 0x01);
    BMSlightOn();
  }
  if(code == 26) { // IMD error
    IMDlightOn();
  }
    
  
  if(PRINT_MODE == 64) {
    Serial.write(32);
    Serial.print(code);
  }
  else {
    Serial.print("EPIC ERROR -- ");
    Serial.println(code);
  }
    
  while(DEBUG_ACTIVATE != 42) { // return if debug mode is on.
                                    //  "shut them ALL down!!!"
      EVDC::shutThemAllDown(datCAN);//    - C-3PO, on trash compactors containing friends
      RPi::giveError(datCAN, code);
      digitalWrite(software_shutdown_control, LOW);
      digitalWrite(AIRdcdc, LOW);
      delay(200);
  }
  digitalWrite(readyToDriveSound, HIGH); // even if in debug mode, turn on RTD sound so that people know there is an error
}

void alertError(MCP_CAN& datCAN, int code) { // alert errors are for things not serious enough to shut down 
  RPi::giveError(datCAN, code);
  if(PRINT_MODE == 64) {
    Serial.write(32);
    Serial.print(code);
  }
  else {
    Serial.print("EPIC ERROR -- ");
    Serial.println(code);
  }
}

