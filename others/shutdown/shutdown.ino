void ShutDownNormal() {
  RPi::giveProgression(8);
  digitalWrite(Air4, LOW);
  digitalWrite(Air2, LOW);
  digitalWrite(Air3, LOW);
  digitalWrite(Air1, LOW);
  digitalWrite(discharge, HIGH);
  delay(8000);
  digitalWrite(discharge, LOW);
  digitalWrite(TSMasterRelay, LOW);
}
