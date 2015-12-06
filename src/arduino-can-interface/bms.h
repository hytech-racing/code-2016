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
	
    float getPackVoltage(char*);
    float getCurrent(char*);
    float getHighTemp(char*);
	float getStateOfCharge(char*);
	float getHighResistance(char*);
	float getLowVoltage(char*);
	float getPowerDischargeLimit(char*);
}
/*
float BMSstateOfCharge;
float BMScurrent;
float BMSdischargeLimit;
float BMSchargeLimit;
float BMShighestTemp;
float BMSlowcellvoltage;
*/
