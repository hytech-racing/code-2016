#include <SPI.h>
#include "mcp_can.h"

#pragma once

/**
 * Functions for sending and receiving data from the Motor Controller
 * Shashank S
 */

namespace MC {

    float getTemp(char[]);
    float getRPM(char[]);
    float getCurrent(char[]);

    void sendRPM(MCP_CAN* can, float data);

}
