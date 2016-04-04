#ifndef SUPERPLEX
#define SUPERPLEX

boolean getMultiplexerDigital(char selection) {
  digitalWrite(MULTIPLEXER_SELECT_0, LOW);
  digitalWrite(MULTIPLEXER_SELECT_1, LOW);
  digitalWrite(MULTIPLEXER_SELECT_2, LOW);
  digitalWrite(MULTIPLEXER_SELECT_3, LOW);
  
  if((selection & 0x01) == 0x01) {
    digitalWrite(MULTIPLEXER_SELECT_0, HIGH);
  }
  if((selection & 0x02) == 0x02) {
    digitalWrite(MULTIPLEXER_SELECT_1, HIGH);
  }
  if((selection & 0x04) == 0x04) {
    digitalWrite(MULTIPLEXER_SELECT_2, HIGH);
  }
  if((selection & 0x08) == 0x08) {
    digitalWrite(MULTIPLEXER_SELECT_3, HIGH);
  }
  delayMicroseconds(1) ; //for propagation delay
  return digitalRead(MULTIPLEXER_INPUT);
}



int getMultiplexerAnalog(char selection) {
  digitalWrite(MULTIPLEXER_SELECT_0, LOW);
  digitalWrite(MULTIPLEXER_SELECT_1, LOW);
  digitalWrite(MULTIPLEXER_SELECT_2, LOW);
  digitalWrite(MULTIPLEXER_SELECT_3, LOW);
  
  if((selection & 0x01) == 0x01) {
    digitalWrite(MULTIPLEXER_SELECT_0, HIGH);
  }
  if((selection & 0x02) == 0x02) {
    digitalWrite(MULTIPLEXER_SELECT_1, HIGH);
  }
  if((selection & 0x04) == 0x04) {
    digitalWrite(MULTIPLEXER_SELECT_2, HIGH);
  }
  if((selection & 0x08) == 0x08) {
    digitalWrite(MULTIPLEXER_SELECT_3, HIGH);
  }
  delayMicroseconds(1) ; //for propagation delay
  return analogRead(MULTIPLEXER_INPUT);
}
  #endif
