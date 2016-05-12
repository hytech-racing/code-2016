#ifndef CAN_MESSAGE_H
#define CAN_MESSAGE_H

#include <stdint.h>

class can_message
{
public:
    

    can_message();
    ~can_message();
    
  public:
    unsigned char* command_data;
    int id;
};

#endif
