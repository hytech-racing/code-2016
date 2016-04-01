#pragma once
#include "mcp_can.h"


/**
 * Functions for sending and receiving data from the Motor Controller
 * Shashank S
 */
 
namespace MC {
    const int Message_Phase_Temp = 0x0A0;
    float getAveragePhaseTemp(unsigned char *);
    float getMaxPhaseTemp(unsigned char*);
    float getGateDriverTemp(unsigned char *);
    const int Message_Internal_Temp =  0x0A1;
    float getControlBoardTemp(unsigned char *);
    float getThermistor1Temp(unsigned char *);
    float getThermistor2Temp(unsigned char *);
    float getThermistor3Temp(unsigned char *);
    const int Message_Motor_Temp =  0x0A2;
    float getMotorTemp(unsigned char *);
    // 0x0A4
    bool isForwardSwitchOn(unsigned char *);
    bool isReverseSwitchOn(unsigned char *);
    bool isBrakeSwitchOn(unsigned char *);
    bool isRegenOn(unsigned char *);
    bool isIgnitionOn(unsigned char *);
    bool isStartSwitchOn(unsigned char *);
    const int Message_Motor_Speed = 0x0A5;
    int getMotorRPM(unsigned char *);
    const int Message_Motor_Current =  0x0A6;
    float getPhaseACurrent(unsigned char *);
    float getPhaseBCurrent(unsigned char *);
    float getPhaseCCurrent(unsigned char *);
    float getDCBusCurrent(unsigned char *);
    // 0x0A7
    float getDCBusVoltage(unsigned char *);
    float getOutputVoltage(unsigned char *);
    float getPhaseABVoltage(unsigned char *);
    float getPhaseBCVoltage(unsigned char *);
    // 0x0AA
    const unsigned char STATE_POWER_ON = 0;
    const unsigned char STATE_STOP = 1;
    const unsigned char STATE_OPEN_LOOP = 2;
    const unsigned char STATE_CLOSED_LOOP = 3;
    const unsigned char STATE_WAIT = 4;
    const unsigned char STATE_IDLE_RUN = 8;
    const unsigned char STATE_IDLE_STOP = 9;
    unsigned char getInverterState(unsigned char *);
    bool isInverterOn(unsigned char *);
    // 0x0AB
    unsigned long getPOSTFault(unsigned char *);
    unsigned long getRUNFault(unsigned char *);
    // 0x0AC
    unsigned long getElapsedTime(unsigned char *);
    //other things
    void shutThemAllDown(MCP_CAN&);
}
