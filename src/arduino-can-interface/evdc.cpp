#include "EVDC.h"

EVDC::getError(char* data) {
	return int(data[0]);
}

EVDC::getButtons(char* data) {
	return int(data[1]);
}