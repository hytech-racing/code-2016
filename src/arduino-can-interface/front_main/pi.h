// Raspberry Pi Header
#pragma once
#include "mcp_can.h"

namespace RPi {
	void giveProgression(MCP_CAN&, int);
	void giveError(MCP_CAN&, int);
	const int piAddress = 0x083; // 3 is like 'pi', right?
}
