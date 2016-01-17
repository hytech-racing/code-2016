#include "mc.h"
#include <cstring>

// 0x0A0
float MC::getAveragePhaseTemp(unsigned char *data) {
    short phaseA, phaseB, phaseC;
    memcpy(&phaseA, &data[0], sizeof(short));
    memcpy(&phaseB, &data[2], sizeof(short));
    memcpy(&phaseC, &data[4], sizeof(short));
    return (phaseA + phaseB + phaseC) / 30.0f;
}

float MC::getGateDriverTemp(unsigned char *data) {
    short gateDriverTemp;
    memcpy(&gateDriverTemp, &data[6], sizeof(short));
    return gateDriverTemp / 10.0f;
}

// 0x0A1
float MC::getControlBoardTemp(unsigned char *data) {
    short boardTemp;
    memcpy(&boardTemp, &data[0], sizeof(short));
    return boardTemp / 10.0f;
}

float MC::getThermistor1Temp(unsigned char *data) {
    short t1Temp;
    memcpy(&t1Temp, &data[2], sizeof(short));
    return t1Temp / 10.0f;
}

float MC::getThermistor2Temp(unsigned char *data) {
    short t2Temp;
    memcpy(&t2Temp, &data[4], sizeof(short));
    return t2Temp / 10.0f;
}

// 0x0A2
float MC::getMotorTemperature(unsigned char *data) {
    short motorTemp;
    memcpy(&motorTemp, &data[4], sizeof(short));
    return motorTemp / 10.0f;
}

// 0x0A4
bool MC::isForwardSwitchOn(unsigned char *data) {
    return data[0];
}

bool MC::isReverseSwitchOn(unsigned char *data) {
    return data[1];
}

bool MC::isBrakeSwitchOn(unsigned char *data) {
    return data[2];
}

bool MC::isRegenOn(unsigned char *data) {
    return !((bool) data[3]);
}

bool MC::isIgnitionOn(unsigned char *data) {
    return data[4];
}
bool MC::isStartSwitchOn(unsigned char *data) {
    return data[5];
}

// 0x0A5
int MC::getMotorRPM(unsigned char *data) {
    short motorRPM;
    memcpy(&motorRPM, &data[2], sizeof(short));
    return motorRPM;
}

// 0x0A6
float MC::getPhaseACurrent(unsigned char *data) {
    short aI;
    memcpy(&aI, &data[0], sizeof(short));
    return aI / 10.0f;
}

float MC::getPhaseBCurrent(unsigned char *data) {
    short bI;
    memcpy(&bI, &data[2], sizeof(short));
    return bI / 10.0f;
}

float MC::getPhaseCCurrent(unsigned char *data) {
    short cI;
    memcpy(&cI, &data[4], sizeof(short));
    return cI / 10.0f;
}

float MC::getDCBusCurrent(unsigned char *data) {
    short dcI;
    memcpy(&dcI, &data[6], sizeof(short));
    return dcI / 10.0f;
}

// 0x0A7
float MC::getDCBusVoltage(unsigned char *data) {
    short dcV;
    memcpy(&dcV, &data[0], sizeof(short));
    return dcV / 10.0f;
}

float MC::getOutputVoltage(unsigned char *data) {
    short oV;
    memcpy(&oV, &data[2], sizeof(short));
    return oV / 10.0f;
}

float MC::getPhaseABVoltage(unsigned char *data) {
    short abV;
    memcpy(&abV, &data[4], sizeof(short));
    return abV / 10.0f;
}

float MC::getPhaseBCVoltage(unsigned char *data) {
    short bcV;
    memcpy(&bcV, &data[6], sizeof(short));
    return bcV / 10.0f;
}

// 0x0AA
unsigned char MC::getInverterState(unsigned char *data) {
    return data[2];
}

bool MC::isInverterOn(unsigned char *data) {
    return data[6] & 0x1;
}

// 0x0AB
unsigned long MC::getPOSTFault(unsigned char *data) {
    unsigned long errorCode;
    memcpy(&errorCode, &data[0], sizeof(unsigned long));
    return errorCode;
}

unsigned long MC::getRUNFault(unsigned char *data) {
    unsigned long errorCode;
    memcpy(&errorCode, &data[4], sizeof(unsigned long));
    return errorCode;
}

// 0x0AC
unsigned long MC::getElapsedTime(unsigned char *data) {
    unsigned long elapsedTime;
    memcpy(&elapsedTime, &data[4], sizeof(unsigned long));
    return (unsigned long) (elapsedTime * 0.003f);
}
