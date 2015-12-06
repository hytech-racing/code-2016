#pragma once

/**
 * Functions for sending and receiving data from the Motor Controller
 * Shashank S
 */

namespace MC {

    float getPhaseTemp(char*);
    float getMotorTemp(char*);
    float getRPM(char*);
    float getCurrent(char*);
	int Message_Phase_temp = 0xA0;
	int Message_Motor_Temp = 0xA2;
	int Message_Motor_Speed= 0xA5;
	int Message_Motor_Current = 0xA6
}
