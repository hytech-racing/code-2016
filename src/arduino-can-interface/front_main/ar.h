#include <SPI.h>
#include "mcp_can.h"

#pragma once

/**
 * Functions for sending and receiving data from the rear Arduino
 * Sanjay S
 */

namespace AR {
	int getError(unsigned char*);
	const int Message = 0xA20; // 'Arduino 2'
}
