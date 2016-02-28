

void shutDownNormal(MCP_CAN& datCAN) {
  RPi::giveProgression(datCAN, 8);
  digitalWrite(AIRdcdc, LOW);
  digitalWrite(discharge, LOW);
  digitalWrite(TSMasterRelay, LOW);
  delay(3000);
  digitalWrite(discharge, LOW);
}

void shutdownError(MCP_CAN& datCAN, int code) {
  RPi::giveError(datCAN, code);
  //MC::shutThemAllDown(datCAN); //  "shut them ALL down!!!"
  EVDC::shutThemAllDown(datCAN);//    - C-3PO, on trash compactors containing friends
  digitalWrite(TSMasterRelay, LOW);
  digitalWrite(AIRdcdc, LOW);
  digitalWrite(discharge, LOW); // discharge is on by default
  Serial.print("EPIC ERROR -- ");
  Serial.println(code);
  while(1) {
      EVDC::shutThemAllDown;
      RPi::giveError(datCAN, code);
      delay(200);
  }

}
void alertError(MCP_CAN& datCAN, int code) { // alert errors are for things not serious enough to shut down 
  RPi::giveError(datCAN, code);
  Serial.print("ALERT ERROR -- ");
  Serial.println(code);
}

