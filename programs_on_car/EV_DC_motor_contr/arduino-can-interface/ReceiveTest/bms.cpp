#include "bms.h"

float BMS::getVoltage(unsigned char* data) {
    unsigned char high = data[2];
    unsigned char low = data[3];

    float volts = (high << 8) | low;
    return volts;
}

float BMS::getCurrent(unsigned char* data) {
    unsigned char high = data[0];
    unsigned char low = data[1];

    float current = (high << 8) | low;
    return current;
}

float BMS::getTemp(unsigned char* data) {
    unsigned char uTemp = data[4]; //Replace with actual value when we know what it is
    byte temp = (byte) uTemp;
    float fTemp = (float) temp;

    return fTemp;
}
