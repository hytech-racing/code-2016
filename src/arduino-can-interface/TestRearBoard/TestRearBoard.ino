int bigNumber;

void setup() {
  Serial.begin(9600);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
}

void loop() {
  digitalWrite(3, HIGH);
  digitalWrite(4, HIGH);
  digitalWrite(5, HIGH);
  digitalWrite(6, HIGH);
  digitalWrite(7, HIGH);
  digitalWrite(8, HIGH);
  
  if((bigNumber & 0x01) == 0x01) { // pins are active low
    digitalWrite(8, LOW);
    digitalWrite(5, LOW);
  }
  
  if((bigNumber & 0x02) == 0x02) {
    digitalWrite(7, LOW);
    digitalWrite(4, LOW);
  }
  
  
 if((bigNumber & 0x04) == 0x04) {
   digitalWrite(6, LOW);
   digitalWrite(3, LOW);
 }
 
 delay(10);
 Serial.print("Combo Number: ");
 Serial.println(bigNumber);
 Serial.print("Chip 1  ");
 Serial.println(analogRead(1));
 Serial.print("Chip 2  ");
 Serial.println(analogRead(3));
 delay(200);
 bigNumber++;
 bigNumber &= 0x0007;

}
