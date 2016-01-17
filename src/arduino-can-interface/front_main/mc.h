#pragma once

/**
 * Functions for sending and receiving data from the Motor Controller
 * Shashank S
 */

namespace MC {
    // 0x0A0
    float getAveragePhaseTemp(unsigned char *);
    float getGateDriverTemp(unsigned char *);
    // 0x0A1
    float getControlBoardTemp(unsigned char *);
    float getThermistor1Temp(unsigned char *);
    float getThermistor2Temp(unsigned char *);
    float getThermistor3Temp(unsigned char *);
    // 0x0A2
    float getMotorTemperature(unsigned char *);
    // 0x0A4
    bool isForwardSwitchOn(unsigned char *);
    bool isReverseSwitchOn(unsigned char *);
    bool isBrakeSwitchOn(unsigned char *);
    bool isRegenOn(unsigned char *);
    bool isIgnitionOn(unsigned char *);
    bool isStartSwitchOn(unsigned char *);
    // 0x0A5
    int getMotorRPM(unsigned char *);
    // 0x0A6
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
    unsigned char STATE_POWER_ON = 0;
    unsigned char STATE_STOP = 1;
    unsigned char STATE_OPEN_LOOP = 2;
    unsigned char STATE_CLOSED_LOOP = 3;
    unsigned char STATE_WAIT = 4;
    unsigned char STATE_IDLE_RUN = 8;
    unsigned char STATE_IDLE_STOP = 9;
    unsigned char getInverterState(unsigned char *);
    bool isInverterOn(unsigned char *);
    // 0x0AB
    unsigned long getPOSTFault(unsigned char *);
    unsigned long getRUNFault(unsigned char *);
    // 0x0AC
    unsigned long getElapsedTime(unsigned char *);
}
