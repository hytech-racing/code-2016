#include <SPI.h>
#include "mcp_can.h"

#pragma once

/**
 * Functions for sending and receiving data from the Battery Management System
 * Ryan Gallaway
 */

namespace BMS {
    double getVoltage(char[]);
    double getCurrent(char[]);
    double getTemp(char[]);
}
