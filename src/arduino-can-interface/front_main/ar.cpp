#include "ar.h"

int AR::getError(unsigned char* data) {
	return int(data[0]);
}
