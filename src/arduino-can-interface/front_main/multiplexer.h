#ifndef SUPERPLEX
#define SUPERPLEX

boolean getMultiplexerDigital(char selection) {
  digitalWrite(MULTIPLEXER_SELECT_0, HIGH);
  digitalWrite(MULTIPLEXER_SELECT_1, HIGH);
  digitalWrite(MULTIPLEXER_SELECT_2, HIGH);
  digitalWrite(MULTIPLEXER_SELECT_3, HIGH);
  
  if((selection & 0x01) == 0x01) {
    digitalWrite(MULTIPLEXER_SELECT_0, LOW);
  }
  if((selection & 0x02) == 0x02) {
    digitalWrite(MULTIPLEXER_SELECT_1, LOW);
  }
  if((selection & 0x04) == 0x04) {
    digitalWrite(MULTIPLEXER_SELECT_2, LOW);
  }
  if((selection & 0x08) == 0x08) {
    digitalWrite(MULTIPLEXER_SELECT_3, LOW);
  }
  delayMicroseconds(1) ; //for propagation delay
  return digitalRead(MULTIPLEXER_INPUT);
}



int getMultiplexerAnalog(char selection) {
  digitalWrite(MULTIPLEXER_SELECT_0, HIGH);
  digitalWrite(MULTIPLEXER_SELECT_1, HIGH);
  digitalWrite(MULTIPLEXER_SELECT_2, HIGH);
  digitalWrite(MULTIPLEXER_SELECT_3, HIGH);
  
  if((selection & 0x01) == 0x01) {
    digitalWrite(MULTIPLEXER_SELECT_0, LOW);
  }
  if((selection & 0x02) == 0x02) {
    digitalWrite(MULTIPLEXER_SELECT_1, LOW);
  }
  if((selection & 0x04) == 0x04) {
    digitalWrite(MULTIPLEXER_SELECT_2, LOW);
  }
  if((selection & 0x08) == 0x08) {
    digitalWrite(MULTIPLEXER_SELECT_3, LOW);
  }
  delayMicroseconds(1) ; //for propagation delay
  return analogRead(MULTIPLEXER_INPUT);
}
  #endif
