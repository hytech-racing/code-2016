#include "mcp_can.h"

#pragma once

/**
 * Functions for sending and receiving data from the EVDC
 * Maxim M
 */
 
 // Error Codes: 2 means there is an error. 1 means that the EVDC is off (has not been launched with the GoForLaunch function)

namespace EVDC {
	const int Message = 0x0CD; // get it? because it's short for driver controls XD kind of
        const int transmitMessage = 0x08D; // message for transmitting to the EVDC
	int getError(unsigned char*);
	int getButtons(unsigned char*);
        int getBrakes(unsigned char*);
        void goForLaunch(MCP_CAN&);
        void shutThemAllDown(MCP_CAN&);
        void calmTheFreakDown(MCP_CAN&);
}
