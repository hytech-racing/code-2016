j#ifndef CAN_MESSAGE_H
#define CAN_MESSAGE_H

#include <stdint.h>

class can_message
{
public:
    union command_data
    {
        uint8_t bytes[8];
        uint16_t shorts[4];
    };

    can_message();
    ~can_message();

    int get_id();
    command_data get_data();

protected:
    command_data data;
    int id;
};

#endif
