#include "bms.h"

float BMS::getVoltage(char[] data) {
    unsigned char high = data[2];
    unsigned char low = data[3];

    float volts = (high << 8) | low;
    return volts;
}

float BMS::getCurrent(char[] data) {

}

float BMS::getTemp(char[] data) {

}
