// Raspberry Pi Header
#pragma once
#include "mcp_can.h"

namespace RPi {
	void giveProgression(MCP_CAN&, int);
	void giveError(MCP_CAN&, int);
	int piAddress = 0x314; // it looks kind of like 'pi', right?
}
