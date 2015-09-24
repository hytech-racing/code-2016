#include "mc.h"
#include <cstring>
#include <cmath>

float MC::getTemp(char* data) {
    float res;
    memcpy(&res, &data[4], sizeof(float));
    return res;
}

float MC::getRPM(char* data) {
    float res;
    memcpy(&res, &data[4], sizeof(float));
    return res;
}

float MC::getCurrent(char* data) {
    float real, imag;
    memcpy(&real, &data[0], sizeof(float));
    memcpy(&imag, &data[4], sizeof(float));
    return sqrt(pow(real, 2.f) + pow(imag, 2.f));
}
