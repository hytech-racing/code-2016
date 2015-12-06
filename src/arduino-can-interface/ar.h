#include <SPI.h>
#include "mcp_can.h"

#pragma once

/**
 * Functions for sending and receiving data from the rear Arduino
 * Sanjay S
 */

namespace AR {
	int getError(char*);
	int Message = 0xAB; // because it looks like AR
}
