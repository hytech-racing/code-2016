void ShutDownNormal(mcp_can& datCan) {
  RPi::giveProgression(datCan, 8);
  EEPROM.write(0,0x00);
  digitalWrite(Air4, LOW);
  digitalWrite(Air2, LOW);
  digitalWrite(Air3, LOW);
  digitalWrite(Air1, LOW);
  digitalWrite(discharge, LOW);
  digitalWrite(TSMasterRelay, LOW);
}

void shutdownError(mcp_can& datCan, int code) {
  RPi::giveError(datCan, code);
  EEPROM.write(0,char(code));
  digitalWrite(TSMasterRelay, LOW);
  digitalWrite(Air4, LOW);
  digitalWrite(Air2, LOW);
  digitalWrite(Air3, LOW);
  digitalWrite(Air1, LOW);
  digitalWrite(discharge, LOW); // discharge is on by default
}
void alertError(mcp_can& datCan, int code) { // alert errors are for things not serious enough to shut down 
 RPi::giveError(datCan, code);
}
