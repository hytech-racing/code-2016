#pragma once

/**
 * Functions for sending and receiving data from the Motor Controller
 * Shashank S
 */

namespace MC {

    float getPhaseTemp(char*);
    float getInternalMCTemp(char*);
    float getMotorTemp(char*);
    float getRPM(char*);
    float getCurrent(char*);
    float getInverterRunMode(char*);

//    void sendRPM(MCP_CAN* can, float data);

}
