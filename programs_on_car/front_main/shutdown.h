

void shutDownNormal(MCP_CAN& datCAN) {
  
  
  for(int i = 0; i < 10; i++) { // wait a second to shut everything down
    EVDC::shutThemAllDown(datCAN);
    RPi::giveProgression(datCAN, 8);
    delay(100);
  }
  digitalWrite(AIRdcdc, LOW);
  digitalWrite(software_shutdown_control, LOW);
  Serial.begin(9600);
  Serial.println("shutting down normal");
  while(1) {
      EVDC::shutThemAllDown(datCAN);
      RPi::giveProgression(datCAN, 8);
      Serial.println("shutting down normal");
      delay(200);
  }

}

void shutdownError(MCP_CAN& datCAN, int code) {
  
  unsigned char len;
  unsigned char buf[8];
  
  RPi::giveError(datCAN, code);
  if(DEBUG_ACTIVATE != 42) {
    extendedScan = true;
  }
  boolean resettableError = true;
  
  if(code == 1 || code == 6 || code == 7 || code == 9 || code == 10) {    // 1, 6,7, 9, 10 are BMS errors and write to EEPROM
    EEPROM.write(0, 0x01);
    BMSlightOn();
    IMDlightOff();
    resettableError = false;
  }
  if(code == 26) { // IMD error
    IMDlightOn();
    resettableError = false;
  }
    
  
  if(PRINT_MODE == 64) {
    Serial.write(32);
    Serial.print(code);
  }
  else {
    Serial.print("EPIC ERROR -- ");
    Serial.println(code);
  }
  
  long shutdownBroadcastTimer = millis();
  boolean noReset = true;
  char ARbuttons;
  unsigned char msgReceive[8];

  
  if(DEBUG_ACTIVATE == 42) {
    noReset = false;
  }
  else {
    digitalWrite(software_shutdown_control, LOW);
    digitalWrite(AIRdcdc, LOW);
    Serial.begin(9600);
  }
  
  long errorTimer = millis() + 50;
  
  unsigned char relayStatus = 0x01;
  
  while(noReset) {
    if(PRINT_MODE == 64) {
      Serial.write(32);
      Serial.print(code);
    }
    else {
      Serial.print("EPIC ERROR -- ");
      Serial.println(code);
    }
    if(millis() > errorTimer) {                              //  "shut them ALL down!!!"
      EVDC::shutThemAllDown(datCAN);//    - C-3PO, on trash compactors containing friends
      RPi::giveError(datCAN, code);
      errorTimer = millis() + 50;
    }
    if(CAN_MSGAVAIL == datCAN.checkReceive()) {
      datCAN.readMsgBuf(&len, buf);
      if(datCAN.getCanId() == BMS::Message_1) {
        relayStatus = BMS::getRelayStatus(buf);
      }
    }
    if((relayStatus & 0x01) == 0) {
      BMSlightOn();
      IMDlightOff();
    }
    else if(code == 26) {
      IMDlightOn();
    }
  }
}






void alertError(MCP_CAN& datCAN, int code) { // alert errors are for things not serious enough to shut down 
  RPi::giveError(datCAN, code);
  if(PRINT_MODE == 64) {
    Serial.write(32);
    Serial.print(code);
  }
  else {
    Serial.print("ALERT ERROR -- ");
    Serial.println(code);
  }
}

