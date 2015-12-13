#include "bms.h"

float BMS::getVoltage(unsigned char* data) {
    unsigned char high = data[2];
    unsigned char low = data[3];

    float volts = (high << 8) | low;
    return volts;
}

float BMS::getCurrent(unsigned char* data) {
    unsigned char high = data[0];
    unsigned char low = data[1];

    float current = (high << 8) | low;
    return current;
}

float BMS::getHighTemp(unsigned char* data) {
    unsigned char uTemp = data[4]; //Replace with actual value when we know what it is
    byte temp = (byte) uTemp;
    float fTemp = (float) temp;

    return fTemp;
}


float getStateOfCharge(char* data){
	return float(data[4]);
}
float getHighResistance(char* data){
	int MostSig = data[0]<<(8);
	int resist = mostSig + data[1];
	resist = resist*100;
	return float(resist);
}
float getLowVoltage(char* data){
	int MostSig = data[2]<<(8);
	int volts = MostSig + data[3];
	float needsDivision = float(volts);
	return needsDivision/10000 ; // the units are in ten-thousandths of a volt
}
float getPowerDischargeLimit(char* data){
	int mostSig = data[4]<<(8);
	int maxPower = mostSig + data[5];
	float needsDivision = float(maxPower);
	return needsDivision/10; // the units are in tenths of a kW
	
}
