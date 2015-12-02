void ShutDownNormal(MCP_CAN& datCan) {
  RPi::giveProgression(8, datCan);
  digitalWrite(Air4, LOW);
  digitalWrite(Air2, LOW);
  digitalWrite(Air3, LOW);
  digitalWrite(Air1, LOW);
  digitalWrite(discharge, LOW);
  digitalWrite(TSMasterRelay, LOW);
}

void shutdownError(int code, MCP_CAN& datCan) {
  RPi::giveError(code, datCan);
  digitalWrite(TSMasterRelay, LOW);
  digitalWrite(Air4, LOW);
  digitalWrite(Air2, LOW);
  digitalWrite(Air3, LOW);
  digitalWrite(Air1, LOW);
  digitalWrite(discharge, LOW); // discharge is on by default
}
