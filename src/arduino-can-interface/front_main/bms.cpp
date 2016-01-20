#include "bms.h"

/**
 * 0x001: Pack Information
 */

unsigned char BMS::getRelayStatus(unsigned char* data) {
    return data[0];
}

float BMS::getStateOfCharge(unsigned char* data){
    return float(data[1]) / 2; // units of 0.5%
}

float BMS::getDepthOfDischarge(unsigned char* data) {
    return float(data[2]) / 2; // units of 0.5%
}

float BMS::getPackHealth(unsigned char* data) {
    return float(data[3]);
}

float BMS::getPackCycles(unsigned char* data) {
    unsigned char high = data[4];
    unsigned char low = data[5];

    float cycles = (high << 8) | low;
    return cycles;
}

float BMS::getSupplyVoltage(unsigned char* data) {
    unsigned char high = data[6];
    unsigned char low = data[7];

    float volts = (high << 8) | low;
    return volts / 10; // units are 0.1v
}

/**
 * 0x002: Pack Values 1
 */

float BMS::getPackCCL(unsigned char* data) {
    unsigned char high = data[0];
    unsigned char low = data[1];

    float limit = (high << 8) | low;
    return limit;
}

float BMS::getCurrent(unsigned char* data) {
    unsigned char high = data[2];
    unsigned char low = data[3];

    float current = (high << 8) | low;
    return current / 10; // units are 0.1A
}

float BMS::getPackDCL(unsigned char* data) {
    unsigned char high = data[4];
    unsigned char low = data[5];

    float limit = (high << 8) | low;
    return limit; // units are 0.1A
}

float BMS::getAmpHours(unsigned char* data) {
    unsigned char high = data[6];
    unsigned char low = data[7];

    float amp = (high << 8) | low;
    return amp / 10; // units are 0.1Ahr
}

/**
 * 0x003: Pack Values 2
 */

float BMS::getPackVoltage(unsigned char* data) {
    unsigned char high = data[0];
    unsigned char low = data[1];

    float volts = (high << 8) | low;
    return volts / 10; // units are 0.1V
}

float BMS::getPackOpenVoltage(unsigned char* data) {
    unsigned char high = data[2];
    unsigned char low = data[3];

    float volts = (high << 8) | low;
    return volts / 10; // units are 0.1V
}

float BMS::getPackSumVoltage(unsigned char* data) {
    unsigned char high = data[4];
    unsigned char low = data[5];

    float volts = (high << 8) | low;
    return volts / 10; // units are 0.1V
}

float BMS::getPackResistance(unsigned char* data) {
    unsigned char high = data[6];
    unsigned char low = data[7];

    float res = (high << 8) | low;
    return res / 10; // units are 0.1 Ohm
}

/**
 * 0x004: Temperature Values
 */

float BMS::getHighTemp(unsigned char* data) {
    return float(data[0]);
}

float BMS::getLowTemp(unsigned char* data) {
    return float(data[1]);
}

float BMS::getAvgTemp(unsigned char* data) {
    return float(data[2]);
}

float BMS::getInternalTemp(unsigned char* data) {
    return float(data[3]);
}

/**
 * 0x005: Cell Voltages
 */

float BMS::getHighVoltage(unsigned char* data) {
    unsigned char high = data[0];
    unsigned char low = data[1];

    float volts = (high << 8) | low;
    return volts / 10000; // units are 0.0001 V
}

float BMS::getLowVoltage(unsigned char* data) {
    int MostSig = data[2]<<(8);
    int volts = MostSig + data[3];
    float needsDivision = float(volts);
    return needsDivision/10000 ; // the units are in ten-thousandths of a volt
}

float BMS::getAvgVoltage(unsigned char* data) {
    unsigned char high = data[4];
    unsigned char low = data[5];

    float volts = (high << 8) | low;
    return volts / 10000; // units are 0.0001 V
}

/**
 * 0x006: Cell Open Voltages
 */

 float BMS::getHighOpenVoltage(unsigned char* data) {
    unsigned char high = data[0];
    unsigned char low = data[1];

    float volts = (high << 8) | low;
    return volts / 10000; // units are 0.0001 V
 }

 float BMS::getLowOpenVoltage(unsigned char* data) {
    unsigned char high = data[2];
    unsigned char low = data[3];

    float volts = (high << 8) | low;
    return volts / 10000; // units are 0.0001 V
 }

 float BMS::getAvgOpenVoltage(unsigned char* data) {
    unsigned char high = data[4];
    unsigned char low = data[5];

    float volts = (high << 8) | low;
    return volts / 10000; // units are 0.0001 V
 }

/**
 * 0x007: Cell Resistances
 */

float BMS::getHighResistance(unsigned char* data) {
    unsigned char high = data[0];
    unsigned char low = data[1];

    float res = (high << 8) | low;
    return res / 100; // units are 0.01 milli-ohm
}

float BMS::getLowResistance(unsigned char* data) {
    unsigned char high = data[2];
    unsigned char low = data[3];

    float res = (high << 8) | low;
    return res / 100; // units are 0.01 milli-ohm
}

float BMS::getAvgResistance(unsigned char* data) {
    unsigned char high = data[4];
    unsigned char low = data[5];

    float res = (high << 8) | low;
    return res / 100; // units are 0.01 milli-ohm
}


