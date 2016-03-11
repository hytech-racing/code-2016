boolean getMultiplexerDigital(char selection) {
  if((selection & 0x01 ==) 0x01) {
    digitalWrite(MULTIPLEXER_SELECT_0, LOW);
  }
  if((selection & 0x02 ==) 0x02) {
    digitalWrite(MULTIPLEXER_SELECT_0, LOW);
  }
  if((selection & 0x04 ==) 0x04) {
    digitalWrite(MULTIPLEXER_SELECT_0, LOW);
  }
  if((selection & 0x08 ==) 0x08) {
    digitalWrite(MULTIPLEXER_SELECT_0, LOW);
  }
  delayMicroseconds(1) ; //for propagation delay
  return digitalRead(MULTIPLEXER_INPUT);
}



int getMultiplexerAnalog(char selection) {
  if((selection & 0x01 ==) 0x01) {
    digitalWrite(MULTIPLEXER_SELECT_0, LOW);
  }
  if((selection & 0x02 ==) 0x02) {
    digitalWrite(MULTIPLEXER_SELECT_0, LOW);
  }
  if((selection & 0x04 ==) 0x04) {
    digitalWrite(MULTIPLEXER_SELECT_0, LOW);
  }
  if((selection & 0x08 ==) 0x08) {
    digitalWrite(MULTIPLEXER_SELECT_0, LOW);
  }
  delayMicroseconds(1) ; //for propagation delay
  return analogRead(MULTIPLEXER_INPUT);
}
  
