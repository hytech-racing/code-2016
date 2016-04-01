

void shutDownNormal(MCP_CAN& datCAN) {
  RPi::giveProgression(datCAN, 8);
  digitalWrite(AIRdcdc, LOW);
  digitalWrite(software_shutdown_control, LOW);

}

void shutdownError(MCP_CAN& datCAN, int code) {
  RPi::giveError(datCAN, code);
  //MC::shutThemAllDown(datCAN); //  "shut them ALL down!!!"
  EVDC::shutThemAllDown(datCAN);//    - C-3PO, on trash compactors containing friends
  digitalWrite(software_shutdown_control, LOW);
  digitalWrite(AIRdcdc, LOW);
  Serial.print("EPIC ERROR -- ");
  Serial.println(code);
  while(1) {
      EVDC::shutThemAllDown(datCAN);
      RPi::giveError(datCAN, code);
      delay(200);
  }

}
void alertError(MCP_CAN& datCAN, int code) { // alert errors are for things not serious enough to shut down 
  RPi::giveError(datCAN, code);
  Serial.print("ALERT ERROR -- ");
  Serial.println(code);
}

