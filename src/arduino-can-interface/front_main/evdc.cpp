#include "EVDC.h"

int EVDC::getError(unsigned char* data) {
	return int(data[0]);
}

int EVDC::getButtons(unsigned char* data) {
	return int(data[1]);
}
