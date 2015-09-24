#include "mc.h"
#include <cstring>
#include <cmath>

float MC::getTemp(char* data) {
    float res;
    // store second float (bytes 4-7) of data in res
    memcpy(&res, &data[4], sizeof(float));
    return res;
}

float MC::getRPM(char* data) {
    float res;
    // store second float (bytes 4-7) of data in res
    memcpy(&res, &data[4], sizeof(float));
    return res;
}

float MC::getCurrent(char* data) {
    float real, imag;
    // store first float (bytes 0-3) of data in real
    memcpy(&real, &data[0], sizeof(float));
    // store second float (bytes 4-7) of data in imag
    memcpy(&imag, &data[4], sizeof(float));
    // return magnitude of complex current
    return sqrt(pow(real, 2.f) + pow(imag, 2.f));
}
