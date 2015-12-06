// Raspberry Pi Header

namespace RPi {
	void sendProgression(mcp_can&, int);
	void sendError(mcp_can&, int);
	int piAddress = 0xD1; // it looks kind of like 'pi', right?
}