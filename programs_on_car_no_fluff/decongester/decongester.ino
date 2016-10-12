
#include <SPI.h>
#include <mcp_can.h>
#include <mcp_can_dfs.h>

MCP_CAN MCcan(9); // SEED shield goes to MC
MCP_CAN CarCAN(10); // Sparkfun shield goes to rest of car

int messageA0counter = 0;
int messageA1counter = 0;
int messageA2counter = 0;
int messageA3counter = 0;
int messageA4counter = 0;
int messageA5counter = 0;
int messageA6counter = 0;
int messageA7counter = 0;
int messageAAcounter = 0;
int messageABcounter = 0;
int messageACcounter = 0;

long messageEVDCtimer = 0;
#define MOTOR_CONTR_MESSAGE_DELAY 20 // 50 control messages per second. lols.

boolean nextMessageDisable = false;
unsigned char disable[8] = {0,0,0,0,0,0,0,0};
#define EVDC_TIMEOUT_LIMIT 500 // .5 seconds until an EVDC timeout is declared

long messageTimeoutCheck = 0;


long MCtimeout = 0;


unsigned char tempBuf[8];

unsigned char lastA0message[8];
unsigned char lastA1message[8];
unsigned char lastA2message[8];
unsigned char lastA3message[8];
unsigned char lastA4message[8];
unsigned char lastA5message[8];
unsigned char lastA6message[8];
unsigned char lastA7message[8];
unsigned char lastAAmessage[8];
unsigned char lastABmessage[8];
unsigned char lastACmessage[8];

unsigned char lastEVDCmessage[8] = {0,0,0,0,0,0,0,0};







void setup() {
  Serial.begin(115200);
  
  while(CAN_OK != MCcan.begin(CAN_500KBPS)) {
    Serial.println("MC bus not operational");
    delay(10);
  }
  while(CAN_OK != CarCAN.begin(CAN_500KBPS)){
    Serial.println("car bus not operational");
    delay(10);
  }
  
  CarCAN.init_Filt(0, 0, 0x0C0); // scan only for command messages
  
  Serial.println("MC BUS and Car Bus operational");

}

void loop() {
  
  unsigned char len = 0;
  
  if(CAN_MSGAVAIL == MCcan.checkReceive()) { // if a message is available from the MC, load it into 
                                             // a buffer and increment a counter
    MCcan.readMsgBuf(&len, tempBuf);
    //Serial.println("M");
    /*Serial.print(millis());
    Serial.print(" - ");
    Serial.print(MCcan.getCanId(), HEX);
    Serial.print(": ");
    for(int i = 0; i < 8; i++) {
      Serial.print(tempBuf[i], HEX);
      Serial.print(" ");
    }
    Serial.println("");*/
    switch(MCcan.getCanId()) {
      case 0x0A0:
        for(int i = 0; i < 8; i++) {
          lastA0message[i] = tempBuf[i];
        }  
        messageA0counter++;
        break;
      case 0x0A1:
        for(int i = 0; i < 8; i++) {
          lastA1message[i] = tempBuf[i];
        }
        messageA1counter++;
        break;
      case 0x0A2:
        for(int i = 0; i < 8; i++) {
          lastA2message[i] = tempBuf[i];
        }
        messageA2counter++;
        break;
      case 0x0A3:
        for(int i = 0; i < 8; i++) {
          lastA3message[i] = tempBuf[i];
        }
        messageA3counter++;
        break;
      case 0x0A4:
        for(int i = 0; i < 8; i++) {
          lastA4message[i] = tempBuf[i];
        }
        messageA4counter++;
        break;
      case 0x0A5:
        for(int i = 0; i < 8; i++) {
          lastA5message[i] = tempBuf[i];
        }
        messageA5counter++;
        break;
      case 0x0A6:
        for(int i = 0; i < 8; i++) {
          lastA6message[i] = tempBuf[i];
        }
        messageA6counter++;
        break;
      case 0x0A7:
        for(int i = 0; i < 8; i++) {
          lastA7message[i] = tempBuf[i];
        }
        messageA7counter++;
        break;
      case 0x0AA:
        for(int i = 0; i < 8; i++) {
          lastAAmessage[i] = tempBuf[i];
        }
        Serial.print(0x0AA, HEX);
        Serial.print(": ");
        for(int i = 0; i < 8; i++) {
          Serial.print(tempBuf[i], HEX);
          Serial.print(" ");
        }
        Serial.println("");
        messageAAcounter++;
        break;
      case 0x0AB:
        for(int i = 0; i < 8; i++) {
          lastABmessage[i] = tempBuf[i];
        }
        messageABcounter++;
        break;
      case 0x0AC:
        for(int i = 0; i < 8; i++) {
          lastACmessage[i] = tempBuf[i];
        }
        messageACcounter++;
        break;
      case 0x0AF:
        Serial.print(0x0AF, HEX);
        Serial.print(": ");
        for(int i = 0; i < 8; i++) {
          Serial.print(tempBuf[i], HEX);
          Serial.print(" ");
        }
        Serial.println("");
      default: // Otherwise rebroadcast message on Car CAN Bus
        CarCAN.sendMsgBuf(MCcan.getCanId(), 0, 8, tempBuf);
    }
  }
  
  // here, if the decongester has recieved a certain number of messages,
  // it will put a message out onto the Car CAN BUS
  
  if(messageA0counter > 1) {
    CarCAN.sendMsgBuf(0x0A0, 0, 8, lastA0message);
    messageA0counter = 0;
  }
  if(messageA1counter > 1) {
    CarCAN.sendMsgBuf(0x0A1, 0, 8, lastA1message);
    messageA1counter = 0;
  }
  if(messageA2counter > 1) {
    CarCAN.sendMsgBuf(0x0A2, 0, 8, lastA2message);
    messageA2counter = 0;
  }
  if(messageA3counter > 7) {
    CarCAN.sendMsgBuf(0x0A3, 0, 8, lastA3message);
    messageA3counter = 0;
  }
  if(messageA4counter > 7) {
    CarCAN.sendMsgBuf(0x0A4, 0, 8, lastA4message);
    messageA4counter = 0;
  }
  if(messageA5counter > 7) {
    CarCAN.sendMsgBuf(0x0A5, 0, 8, lastA5message);
    messageA5counter = 0;
  }
  if(messageA6counter > 7) {
    CarCAN.sendMsgBuf(0x0A6, 0, 8, lastA6message);
    messageA6counter = 0;
  }
  if(messageA7counter > 7) {
    CarCAN.sendMsgBuf(0x0A7, 0, 8, lastA7message);
    messageA7counter = 0;
  }
  if(messageAAcounter > 1) {
    CarCAN.sendMsgBuf(0x0AA, 0, 8, lastAAmessage);
    Serial.println("********** MC Internal States **********");
    int vsmState = (lastAAmessage[0] << 8) | lastAAmessage[1];
    Serial.print("VSM State: ");
    Serial.print(vsmState);
    switch(vsmState) {
      case 0: 
        Serial.println(" (VSM Start State)");
        break;
      case 1:
        Serial.println(" (Pre-charge Init State)");
        break;
      case 2:
        Serial.println(" (Pre-charge Active State)");
        break;
      case 3:
        Serial.println(" (Pre-charge Complete State)");
        break;
      case 4:
        Serial.println(" (VSM Wait State)");
        break;
      case 5:
        Serial.println(" (VSM Ready State)");
        break;
      case 6:
        Serial.println(" (Motor Running State)");
        break;
      case 7:
        Serial.println(" (Blink Fault Code State)");
        break;
      case 14:
        Serial.println(" (Shutdown in progress)");
        break;
      case 15:
        Serial.println(" (Recycle Power State)");
        break;
    }
    Serial.print("Inverter State: ");
    Serial.print(lastAAmessage[2]);
    switch(lastAAmessage[2]) {
      case 0:
        Serial.println(" (Power On State)");
        break;
      case 1:
        Serial.println(" (Stop State)");
        break;
      case 2:
        Serial.println(" (Open Loop State)");
        break;
      case 3:
        Serial.println(" (Closed Loop State)");
        break;
      case 4:
        Serial.println(" (Wait State)");
        break;
      case 5:
      case 6:
      case 7:
        Serial.println(" (Internal State)");
        break;
      case 8:
        Serial.println(" (Idle Run State)");
        break;
      case 9:
        Serial.println(" (Idle Stop State)");
        break;
      case 10:
      case 11:
      case 12:
        Serial.println(" (Internal State)");
    }
    Serial.print("Relay State (last 6 bits): ");
    Serial.println(lastAAmessage[3], HEX);
    Serial.print("Inverter Run Mode: ");
    int runMode = lastAAmessage[4] & 0x1;
    Serial.print(runMode);
    switch(runMode) {
      case 0:
        Serial.println(" (Torque Mode)");
        break;
      case 1:
        Serial.println(" (Speed Mode)");
        break;
    }
    int dischargeState = (lastAAmessage[4] & 0xE0) >> 5;
    Serial.print("Inverter Active Discharge State: ");
    Serial.print(dischargeState);
    switch(dischargeState) {
      case 0:
        Serial.println(" (Discharge Disabled)");
        break;
      case 1:
        Serial.println(" (Discharge Enabled, waiting)");
        break;
      case 2:
        Serial.println(" (Performing Speed Check)");
        break;
      case 3:
        Serial.println(" (Discharge Actively Occurring)");
        break;
      case 4:
        Serial.println(" (Discharge Completed)");
        break;
    }
    int cmdMode = lastAAmessage[5];
    Serial.print("Inverter Command Mode: ");
    Serial.print(cmdMode);
    switch(cmdMode) {
      case 0:
        Serial.println(" (CAN Mode)");
        break;
      case 1:
        Serial.println(" (VSM Mode)");
        break;
    }
    int enableState = lastAAmessage[6] & 0x1;
    Serial.print("Inverter Enable State: ");
    Serial.print(enableState);
    switch(enableState) {
      case 0:
        Serial.println(" (DISABLED)");
        break;
      case 1:
        Serial.println(" (ENABLED)");
        break;
    }
    int lockoutState = (lastAAmessage[6] & 0x80) >> 7;
    Serial.print("Inverter Lockout State: ");
    Serial.print(lockoutState);
    switch(lockoutState) {
      case 0:
        Serial.println(" (Lockout OFF - inverter can be enabled)");
        break;
      case 1:
        Serial.println(" (Lockout ON - inverter cannot be enabled)");
        break;
    }
    int direc = lastAAmessage[7];
    Serial.print("Direction Command: ");
    Serial.print(direc);
    switch(direc) {
      case 0:
        Serial.println(" (Reverse/Stop)");
        break;
      case 1:
        Serial.println(" (Forward)");
        break;
    }
    messageAAcounter = 0;
  }
  if(messageABcounter > 1) {
    CarCAN.sendMsgBuf(0x0AB, 0, 8, lastABmessage);
    messageABcounter = 0;
  }
  if(messageACcounter > 7) {
    CarCAN.sendMsgBuf(0x0AC, 0, 8, lastACmessage);
    messageACcounter = 0;
  }
  
  // here, if a message is available on Car CAN bus,
  // it is rebroadcast on MC CAN bus
  
  if(CAN_MSGAVAIL == CarCAN.checkReceive()) {
    CarCAN.readMsgBuf(&len, tempBuf);
    MCcan.sendMsgBuf(CarCAN.getCanId(), 0, 8, tempBuf);
  }
  
}
