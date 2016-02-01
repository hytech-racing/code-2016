#include <SPI.h>
#include "mcp_can.h"

#pragma once

/**
 * Functions for sending and receiving data from the Battery Management System
 * Ryan Gallaway
 */

namespace BMS {
    const int Message_1 = 0x01; // state of charge and pack health
    const int Message_2 = 0x02; // current, charge and discharge current limits
    const int Message_3 = 0x03; // pack voltage and open voltage
    const int Message_4 = 0x04; // high temperature
    const int Message_5 = 0x05; // low cell voltage
    const int Message_6 = 0x06; // open cell voltages-  probably dont need
    const int Message_7 = 0x07; // cell resistances- probably dont need

    // 0x001: Pack Information
    unsigned char getRelayStatus(unsigned char*);
    float getStateOfCharge(unsigned char*);
    float getDepthOfDischarge(unsigned char*);
    float getPackHealth(unsigned char*);
    float getPackCycles(unsigned char*);
    float getSupplyVoltage(unsigned char*);

    // 0x002: Pack Values 1
    float getPackCCL(unsigned char*);
    float getCurrent(unsigned char*);
    float getPackDCL(unsigned char*);
    float getAmpHours(unsigned char*);

    // 0x003: Pack Values 2
    float getPackVoltage(unsigned char*);
    float getPackOpenVoltage(unsigned char*);
    float getPackSumVoltage(unsigned char*);
    float getPackResistance(unsigned char*);

    // 0x004: Temperature Values
    float getHighTemp(unsigned char*);
    float getLowTemp(unsigned char*);
    float getAvgTemp(unsigned char*);
    float getInternalTemp(unsigned char*);

    // 0x005: Cell Voltages
    float getHighVoltage(unsigned char*);
    float getLowVoltage(unsigned char*);
    float getAvgVoltage(unsigned char*);

    // 0x006: Cell Open Voltages
    float getHighOpenVoltage(unsigned char*);
    float getLowOpenVoltage(unsigned char*);
    float getAvgOpenVoltage(unsigned char*);

    // 0x007: Cell Resistances
    float getHighResistance(unsigned char*);
    float getLowResistance(unsigned char*);
    float getAvgResistance(unsigned char*);
    
    boolean getErrors();
}
/*
float BMSstateOfCharge;
float BMScurrent;
float BMSdischargeLimit;
float BMSchargeLimit;
float BMShighestTemp;
float BMSlowcellvoltage;
*/
