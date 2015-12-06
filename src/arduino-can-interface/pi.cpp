// raspberry pi class implementation

RPi::sendProgression(mcp_can& datCan, int progress) {
	char msgBuf[8];
	msgBuf[0] = byte(progress);
	datCan.sendMsgBuf(piAddress, 0, 8, msgBuf);
}

RPi::sendError(mcp_can& datCan, int error) {
	char msgBuf[8];
	msgBuf[0] = byte(error);
	msgBuf[1] = 0x01;
	datCan.sendMsgBuf(piAddress, 0, 8, msgBuf);
}