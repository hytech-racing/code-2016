#include <SPI.h>


#include <mcp_can.h>
#include <mcp_can_dfs.h>

// change can parameter

#define SPI_CS_PIN 10

#define WRITE_ON 1

#define PARAMETER_ADDRESS 101

#define PARAMETER_NEW_VAL 1600

unsigned char changeMessage[8] = {0,0,0,0,0,0,0,0};

MCP_CAN MCcan(SPI_CS_PIN);

void setup() {
  
  Serial.begin(9600);
  
   while(CAN_OK != MCcan.begin(CAN_500KBPS)) {
    Serial.println("MC bus not operational");
    delay(10);
  }

  MCcan.init_Filt(0, 0, 0x0C2);
  
  changeMessage[2] = WRITE_ON; // write new value
  
  changeMessage[0] = int(PARAMETER_ADDRESS) & 0xFF;
  
  changeMessage[1] = int(PARAMETER_ADDRESS)>>(8);
  
  changeMessage[4] = int(PARAMETER_NEW_VAL) & 0xFF;
  
  changeMessage[5] = int(PARAMETER_NEW_VAL)>>(8);
  
  Serial.println("send a byte");
  while(!Serial.available()) {
    delay(10);
  }
  
  Serial.println("byte sent");
  
  

}

void loop() {
  
  MCcan.sendMsgBuf(0x0C1, 0, 8, changeMessage);
  
  Serial.println("sent");
  
  long dumbTimer = millis() + 1000;// give it one second to respond
  
  unsigned char len;
  
  unsigned char buf[8];
  
  boolean done = false;
  
  while(dumbTimer > millis()) {
    if(CAN_MSGAVAIL == MCcan.checkReceive()) {
      MCcan.readMsgBuf(&len, buf);
      if(MCcan.getCanId() == 0x0C2) {
        for(int i = 0; i < 8; i++){
          Serial.print(buf[i]);
          Serial.write(32);
        }
        Serial.println(" ");
        done = true;
      }
    }
  }
  
  while(done){
     delay(10);
  }   
 
}
