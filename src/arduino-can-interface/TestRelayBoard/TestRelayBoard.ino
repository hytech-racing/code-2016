#define pushbutton 3
#define dcdcControl 9
#define softwareShutdown 5
#define rtDrive 7



void setup() {
  // put your setup code here, to run once:
pinMode(9, OUTPUT);
pinMode(7, OUTPUT);
pinMode(5, OUTPUT);
pinMode(3, OUTPUT);
Serial.begin(9600);
}

void loop() {
  Serial.println("standard tests");
  delay(1000);
  digitalWrite(softwareShutdown, HIGH);
  delay(1000);
  digitalWrite(pushbutton, HIGH);
  delay(1000);
  digitalWrite(pushbutton, LOW);
  delay(1000);
  digitalWrite(dcdcControl, HIGH);
  Serial.println("dcdc ON");
  delay(5000);
  digitalWrite(dcdcControl, LOW);
  Serial.println("dcdc OFF");
  delay(5000);
  digitalWrite(softwareShutdown, LOW);
  delay(1000);
  
  digitalWrite(softwareShutdown, LOW);
  Serial.println("test relay shutdown low");
  for(int i = 0; i < 10; i++) {
    digitalWrite(pushbutton, HIGH);
    delay(1000);
    digitalWrite(pushbutton, LOW);
    delay(1000);
  }
  digitalWrite(softwareShutdown, HIGH);
  Serial.println("test relay shutdown high");
  for(int i = 0; i < 10; i++) {
    digitalWrite(pushbutton, HIGH);
    delay(1000);
    digitalWrite(pushbutton, LOW);
    delay(1000);
  }
  digitalWrite(softwareShutdown, LOW);
  // put your main code here, to run repeatedly:

}
