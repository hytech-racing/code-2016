
void shutDownNormal(MCP_CAN& datCAN) {
  RPi::giveProgression(datCAN, 8);
  digitalWrite(Air4, LOW);
  digitalWrite(Air2, LOW);
  digitalWrite(Air3, LOW);
  digitalWrite(Air1, LOW);
  digitalWrite(discharge, LOW);
  digitalWrite(TSMasterRelay, LOW);
}

void shutdownError(MCP_CAN& datCAN, int code) {
  RPi::giveError(datCAN, code);
  digitalWrite(TSMasterRelay, LOW);
  digitalWrite(Air4, LOW);
  digitalWrite(Air2, LOW);
  digitalWrite(Air3, LOW);
  digitalWrite(Air1, LOW);
  digitalWrite(discharge, LOW); // discharge is on by default
}
void alertError(MCP_CAN& datCAN, int code) { // alert errors are for things not serious enough to shut down 
  RPi::giveError(datCAN, code);
}

