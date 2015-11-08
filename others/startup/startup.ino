#include <SPI.h>
#include "mcp_can.h"
#include "mc.h"
#include "evdc.h"
#include "bms.h"
#include "ar.h"
#include "pi.h"


boolean startupSequence(MCP_CAN& lilEngineThatCAN) { // 0 means a normal startup, 1 means BMS resset, 2 means IMD reset
// EVDC get progression button
  //progression: 1-5 normal, 6 is BMS error, 7 is IMD error 

  unsigned char megGet[8];
  unsigned char msgGive[8];
  unsigned char len;
  boolean IMDswitch;
  boolean progressButton;
  boolean oldProgressButton;
  boolean needsToProgress;
  while(!needsToProgress){
    if(CAN_MSGAVAIL == lilEngineThatCAN.checkReceive()) {
      lilEngineThatCAN.readMsgBuf(&len, msgGet);
      switch(lilEngineThatCAN.getCanId()) {
        case EVDC::Message:
         
          oldProgressButton = progressButton;
          EVDC::getButtonStates(IMDswitch, progressButton, msgReceive);
          
          break;
        default:
          break;
      }
    }
    if(progressButton && !oldProgressButton) {
      needsToProgress = true;
    }
  }
  needsToProgress = false;
  
  
  while(!needsToProgress){
    if(CAN_MSGAVAIL == lilEngineThatCAN.checkReceive()) {
      lilEngineThatCAN.readMsgBuf(&len, msgGet);
      switch(lilEngineThatCAN.getCanId()) {
        case BMS::Message1:
          if(!BMS::getErrors() {
            needsToProgress = true;
          }
          break;
        default:
          break;
      }
    }
    if(progressButton && !oldProgressButton) {
      needsToProgress = true;
    }
  }
  needsToProgress = false;
  
  
  while(!needsToProgress){
    if(!IMD.checkErrors()) {
      needsToProgress = true;
  }
  needsToProgress = false;
         
    while(!needsToProgress){
    if(CAN_MSGAVAIL == lilEngineThatCAN.checkReceive()) {
      lilEngineThatCAN.readMsgBuf(&len, msgGet);
      switch(lilEngineThatCAN.getCanId()) {
        case EVDC::Message:
        
          oldProgressButton = progressButton;
          EVDC::getButtonStates(IMDswitch, progressButton, msgReceive);
          
          break;
        default:
          break;
      }
    }
    if(IMDswitch) {
      needsToProgress = true;
    }
  }
  needsToProgress = false;
  
  
 while(!needsToProgress){
    if(CAN_MSGAVAIL == lilEngineThatCAN.checkReceive()) {
      lilEngineThatCAN.readMsgBuf(&len, msgGet);
      switch(lilEngineThatCAN.getCanId()) {
        case EVDC::Message:
        
          oldProgressButton = progressButton;
          EVDC::getButtonStates(IMDswitch, progressButton, msgReceive);
          
          break;
        default:
          break;
      }
    }
    if(progressButton && !oldProgressButton) {
      needsToProgress = true;
    }
  }
  needsToProgress = false;
  
  while(!needsToProgress){
    if(CAN_MSGAVAIL == lilEngineThatCAN.checkReceive()) {
      lilEngineThatCAN.readMsgBuf(&len, msgGet);
      switch(lilEngineThatCAN.getCanId()) {
        case EVDC::Message:
        
          oldProgressButton = progressButton;
          EVDC::getButtonStates(IMDswitch, progressButton, msgReceive);
          
          break;
        default:
          break;
      }
    }
    if(IMDswitch) {
      needsToProgress = true;
    }
  }
  needsToProgress = false;
       
       
  float brakePedal;
  while(!needsToProgress){
    if(CAN_MSGAVAIL == lilEngineThatCAN.checkReceive()) {
      lilEngineThatCAN.readMsgBuf(&len, msgGet);
      switch(lilEngineThatCAN.getCanId()) {
        case EVDC::Message:
          oldProgressButton = progressButton;
          EVDC::getButtonStates(IMDswitch, progressButton, msgReceive);
          brakePedal = EVDC::getBrakes(msgReceive);
          break;
        default:
          break;
      }
    }
    if(progressButton && !oldProgressButton && (brakePedal > 0.5)){
      needsToProgress = true;
    }
  }
  needsToProgress = false;
        
  digitalWrite(readyToDriveSound, HIGH);
  delay(2000);
  digitalWrite(readyToDriveSound, LOW);
        
        
        
        
        
        
        
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
  while(EVDC::getImdBypassSwitch()) {// the bypass swtich needs to be turned off to progress
    RPi::giveProgression(4);
  }
  RPi::giveProgression(5); // this will display ready to drive
  return true;
}
  
  
  
  
  
