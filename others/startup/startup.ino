boolean startupSequence() { // 0 means a normal startup, 1 means BMS resset, 2 means IMD reset
// EVDC get progression button

//progression: 1-5 normal, 6 is BMS error, 7 is IMD error
  while(!EVDC::getProgressionSwitch()){
    RPi::giveProgression(1);
  }
  digitalWrite(TSMasterRelay, HIGH);

  if(BMS::getError()) {
    RPi::giveProgression(6);
    return false;
  }
  if(getImdError()) {
    RPi::giveProgression(7);
    return false;
  }
  
  
  while(!EVDC::getImdBypassSwitch()) {
    RPi::giveProgression(2);
  }
  digitalWrite(Air1, HIGH);
  delay(500);
  digitalWrite(Air2, HIGH);
  delay(500);
  digitalWrite(Air3, HIGH);
  while(!EVDC::getProgressionSwitch() {
    RPi::giveProgression(3);
  }
  digitalWrite(prechargeRelay, HIGH);
  delay(3000); // check how long
  digitalWrite(Air4, HIGH);
  digitalWrite(prechargeRelay, LOW);
  // now we have high voltage power to the motor controller
  digitalWrite(readyToDriveSound, HIGH);
  while(!EVDC::getImdBypassSwitch()) {
    RPi::giveProgression(4);
  }
  RPi::giveProgression(5); // this will display ready to drive
  return true;
}
  
  
  
  
  
