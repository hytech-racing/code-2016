#include <SPI.h>
#include "mcp_can.h"

#pragma once

/**
 * Functions for sending and receiving data from the Battery Management System
 * Ryan Gallaway
 */

namespace BMS {
    float getVoltage(char[]);
    float getCurrent(char[]);
    float getTemp(char[]);
}
