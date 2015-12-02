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

//    void sendRPM(MCP_CAN* can, float data);
// include definitions for message identifiers, like
// Message_1 = 0x34;
// Message_2 = 0x2D7
// like that
}
