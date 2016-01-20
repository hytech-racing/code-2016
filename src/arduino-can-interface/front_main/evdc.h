#include <SPI.h>
#include "mcp_can.h"

#pragma once

/**
 * Functions for sending and receiving data from the EVDC
 * Maxim M
 */

namespace EVDC {
	const int Message = 0xDC; // get it? because it's short for driver controls XD
	int getError(unsigned char*);
	int getButtons(unsigned char*);
        int getBrakes(unsigned char*);
}
