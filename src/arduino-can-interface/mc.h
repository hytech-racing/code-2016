#pragma once

/**
 * Functions for sending and receiving data from the Motor Controller
 * Shashank S
 */

namespace MC {
    // 0x0A0
    float getModuleATemp(unsigned char *);
    float getModuleBTemp(unsigned char *);
    float getModuleCTemp(unsigned char *);
    float getGateDriverTemp(unsigned char *);
    // 0x0A1
    float getControlBoardTemp(unsigned char *);
    float getThermistor1Temp(unsigned char *);
    float getThermistor2Temp(unsigned char *);
    float getThermistor3Temp(unsigned char *);
    // 0x0A2
    float getMotorTemperature(unsigned char *);
    // 0x0A4
    float isForwardSwitchEnabled(unsigned char *);
    float isReverseSwitchEnabled(unsigned char *);
    float isBrakeSwitchEnabled(unsigned char *);
    float isRegenEnabled(unsigned char *);
    float isIgnitionEnabled(unsigned char *);
    float isStartSwitchEnabled(unsigned char *);
    // 0x0A5
    float getMotorSpeed(unsigned char *);
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
    // 0x0A9
    float get5ReferenceVoltage(unsigned char *);
    float get12ReferenceVoltage(unsigned char *);
    // 0x0AA
    float STATE_POWER_ON = 0;
    float STATE_STOP = 1;
    float STATE_OPEN_LOOP = 2;
    float STATE_CLOSED_LOOP = 3;
    float STATE_WAIT = 4;
    float STATE_IDLE_RUN = 8;
    float STATE_IDLE_STOP = 9;
    float getInverterState(unsigned char *);
}
