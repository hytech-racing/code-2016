#include <SPI.h>
#include "mcp_can.h"

#pragma once

/**
 * Functions for sending and receiving data from the Battery Management System
 * Ryan Gallaway
 */

namespace BMS {
	int Message_1 = 0x01; // state of charge and pack health
	int Message_2 = 0x02; // current, charge and discharge current limits
	int Message_3 = 0x03; // pack voltage and open voltage
	int Message_4 = 0x04; // high temperature
	int Message_5 = 0x05; // low cell voltage
	int Message_6 = 0x06; // open cell voltages-  probably dont need
	int Message_7 = 0x07; // cell resistances- probably dont need

	// 0x001: Pack Information
	float getRelayStatus(char*);
	float getPopulatedCellCount(char*);
	float getStateOfCharge(char*);
	float getDepthOfDischarge(char*);
	float getPackHealth(char*);
	float getPackCycles(char*);
	float getSupplyVoltage(char*);

	// 0x002: Pack Values 1
	float getPackCCL(char*);
    float getCurrent(char*);
    float getPackDCL(char*);
    float getAmpHours(char*);

    // 0x003: Pack Values 2
    float getPackVoltage(char*);
    float getPackOpenVoltage(char*);
    float getPackSumVoltage(char*);
    float getPackResistance(char*);

    // 0x004: Temperature Values
    float getHighTemp(char*);
    float getLowTemp(char*);
    float getAvgTemp(char*);
    float getInternalTemp(char*);

    // 0x005: Cell Voltages
    float getHighVoltage(char*);
	float getLowVoltage(char*);
	float getAvgVoltage(char*);

	// 0x006: Open Cell Voltages
	float getHighOpenVoltage(char*);
	float getLowOpenVoltage(char*);
	float getAvgOpenVoltage(char*);

	// 0x007: Cell Resistances
	float getHighResistance(char*);
	float getLowResistance(char*);
	float getAvgResistance(char*);
}
/*
float BMSstateOfCharge;
float BMScurrent;
float BMSdischargeLimit;
float BMSchargeLimit;
float BMShighestTemp;
float BMSlowcellvoltage;
*/
