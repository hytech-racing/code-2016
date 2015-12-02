#include <SPI.h>
#include "mcp_can.h"

#pragma once

/**
 * Functions for sending and receiving data from the Battery Management System
 * Ryan Gallaway
 */

namespace BMS {
    float getPackVoltage(char*);
    float getCurrent(char*);
    float getHighTemp(char*);
	float getStateOfCharge(char*);
	float getHighResistance(char*);
	float getLowVoltage(char*);
	float getPowerDischargeLimit(char*);
}
