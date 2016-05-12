// raspberry pi class implementation
#include "pi.h"

void RPi::giveProgression(MCP_CAN& datCan, int progress) {
	unsigned char msgBuf[8];
	msgBuf[0] = char(progress);
        msgBuf[1] = 0;
	datCan.sendMsgBuf(piAddress, 0, 8, msgBuf);
}

void RPi::giveError(MCP_CAN& datCan, int error) {
	unsigned char msgBuf[8];
	msgBuf[0] = char(error);
	msgBuf[1] = 0x01;
	datCan.sendMsgBuf(piAddress, 0, 8, msgBuf);
}
