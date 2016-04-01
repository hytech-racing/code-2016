#include "mc.h"

#define max(a,b) ((a)>(b)?(a):(b))

void memcpyInt (short int *dstpp, unsigned char *srcpp) {
    *dstpp &= int(0);
    int MSB = int(srcpp[0]);
    *dstpp |= MSB<<(8);
    *dstpp |= srcpp[1];
}

void memcpyLong (unsigned long *dstpp, unsigned char *srcpp) {
    *dstpp &= int(0);
    unsigned long MSB = long(srcpp[0]);
    unsigned long M2SB = long(srcpp[1]);
    unsigned long L2SB = long(srcpp[2]);
    *dstpp |= MSB<<(24);
    *dstpp |= MSB<<(16);
    *dstpp |= MSB<<(8);
    *dstpp |= srcpp[3];
}  

// 0x0A0
float MC::getAveragePhaseTemp(unsigned char *data) {
    short phaseA, phaseB, phaseC;
    memcpyInt(&phaseA, &data[0]);
    memcpyInt(&phaseB, &data[2]);
    memcpyInt(&phaseC, &data[4]);
    return (phaseA + phaseB + phaseC) / 30.0f;
}

float MC::getMaxPhaseTemp(unsigned char *data) {
    short phaseA, phaseB, phaseC;
    memcpyInt(&phaseA, &data[0]);
    memcpyInt(&phaseB, &data[2]);
    memcpyInt(&phaseC, &data[4]);
    float derpFloat = max(phaseA, max(phaseB, phaseC)) / 10.0f;
    return derpFloat;
}

float MC::getGateDriverTemp(unsigned char *data) {
    short gateDriverTemp;
    memcpyInt(&gateDriverTemp, &data[6]);
    return gateDriverTemp / 10.0f;
}

// 0x0A1
float MC::getControlBoardTemp(unsigned char *data) {
    short boardTemp;
    memcpyInt(&boardTemp, &data[0]);
    return boardTemp / 10.0f;
}

float MC::getThermistor1Temp(unsigned char *data) {
    short t1Temp;
    memcpyInt(&t1Temp, &data[2]);
    return t1Temp / 10.0f;
}

float MC::getThermistor2Temp(unsigned char *data) {
    short t2Temp;
    memcpyInt(&t2Temp, &data[4]);
    return t2Temp / 10.0f;
}

// 0x0A2
float MC::getMotorTemp(unsigned char *data) {
    short motorTemp;
    memcpyInt(&motorTemp, &data[4]);
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
    memcpyInt(&motorRPM, &data[2]);
    return motorRPM;
}

// 0x0A6
float MC::getPhaseACurrent(unsigned char *data) {
    short aI;
    memcpyInt(&aI, &data[0]);
    return aI / 10.0f;
}

float MC::getPhaseBCurrent(unsigned char *data) {
    short bI;
    memcpyInt(&bI, &data[2]);
    return bI / 10.0f;
}

float MC::getPhaseCCurrent(unsigned char *data) {
    short cI;
    memcpyInt(&cI, &data[4]);
    return cI / 10.0f;
}

float MC::getDCBusCurrent(unsigned char *data) {
    short dcI;
    memcpyInt(&dcI, &data[6]);
    return dcI / 10.0f;
}

// 0x0A7
float MC::getDCBusVoltage(unsigned char *data) {
    short dcV;
    memcpyInt(&dcV, &data[0]);
    return dcV / 10.0f;
}

float MC::getOutputVoltage(unsigned char *data) {
    short oV;
    memcpyInt(&oV, &data[2]);
    return oV / 10.0f;
}

float MC::getPhaseABVoltage(unsigned char *data) {
    short abV;
    memcpyInt(&abV, &data[4]);
    return abV / 10.0f;
}

float MC::getPhaseBCVoltage(unsigned char *data) {
    short bcV;
    memcpyInt(&bcV, &data[6]);
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
    memcpyLong(&errorCode, &data[0]);
    return errorCode;
}

unsigned long MC::getRUNFault(unsigned char *data) {
    unsigned long errorCode;
    memcpyLong(&errorCode, &data[4]);
    return errorCode;
}

// 0x0AC
unsigned long MC::getElapsedTime(unsigned char *data) {
    unsigned long elapsedTime;
    memcpyLong(&elapsedTime, &data[4]);
    return (unsigned long) (elapsedTime * 0.003f);
}

void MC::shutThemAllDown(MCP_CAN& CanYouDoTheCANCAN) {
    unsigned char msgBuf[8] = {0,0,0,0,0,0,0,0};
    CanYouDoTheCANCAN.sendMsgBuf(0x0C0, 0, 8, msgBuf);
}
