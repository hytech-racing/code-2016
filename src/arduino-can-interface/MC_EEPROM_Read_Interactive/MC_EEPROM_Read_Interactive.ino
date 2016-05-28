#include <mcp_can.h>
#include <math.h>
#include <SPI.h>
#include <string.h>


boolean stringComplete = false;
String inputString = "";
unsigned char msg[8] = {0, 0, 0, 0, 0, 0, 0, 0};

MCP_CAN CAN(9); // SEED shield goes to MC

void setup() {
  Serial.begin(115200);
  inputString.reserve(200);

  while(CAN_OK != CAN.begin(CAN_500KBPS)) {
    Serial.println("MC bus not operational");
    delay(10);
  }
  Serial.println("MC CAN BUS operational");
  Serial.println("Enter EEPROM value to check");
}


void loop() {
  if (stringComplete) {
    int entry = inputString.toInt();
    Serial.print("Checking ID ");
    Serial.println(entry);
    msg[0] = (unsigned char) entry;
    CAN.sendMsgBuf(0x0C1, 0, 8, msg);
    // clear the string:
    inputString = "";
    stringComplete = false;
  }
  
  if(CAN_MSGAVAIL == CAN.checkReceive()) {
    unsigned char buf[8];
    unsigned char len;
    CAN.readMsgBuf(&len, buf);

    if (CAN.getCanId() == 0x0C2) {
      Serial.print(0x0C2, HEX);
      Serial.print(": ");
      for(int i = 0; i < 8; i++) {
        Serial.print(buf[i], HEX);
        Serial.print(" ");
      }
      Serial.println("");
      /*Serial.print(buf[2]);
      Serial.print(": ");
      Serial.print(buf[5], HEX);
      Serial.print(" ");
      Serial.println(buf[4], HEX);*/
    }
  }
}

void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if (inChar == '\n') {
      stringComplete = true;
    } else {
      inputString += inChar;
    }
  }
}
