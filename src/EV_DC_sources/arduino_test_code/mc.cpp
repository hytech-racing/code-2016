#include "mc.h"
#include <string.h>

float MC::getPhaseTemp(char *data) {
    short phaseA, phaseB, phaseC;
    memcpy(&phaseA, &data[0], sizeof(short));
    memcpy(&phaseB, &data[2], sizeof(short));
    memcpy(&phaseC, &data[4], sizeof(short));
    return (phaseA + phaseB + phaseC) / 30.0f;
}

float MC::getMotorTemp(char *data) {
    short motorTemp;
    memcpy(&motorTemp, &data[4], sizeof(short));
    return motorTemp / 10.0f;
}

float MC::getInternalMCTemp(char *data) {
    short motorTemp;
    memcpy(&motorTemp, &data[0], sizeof(short));
    return motorTemp / 10.0f;
}

float MC::getRPM(char *data) {
    short motorSpeed;
    memcpy(&motorSpeed, &data[2], sizeof(short));
    return (float) motorSpeed;
}

float MC::getCurrent(char *data) {
    short current;
    memcpy(&current, &data[6], sizeof(short));
    return current / 10.0f;
}

float MC::getInverterRunMode(char *data) {
    return (float) (data[4] & 0x01);
}
