#include <SPI.h>

#pragma once

/**
 * Functions for sending and receiving data from the Battery Management System
 * Ryan Gallaway
 */

namespace BMS {
    float getVoltage(unsigned char*);
    float getCurrent(unsigned char*);
    float getTemp(unsigned char*);
}
