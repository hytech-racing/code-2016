#include "can_message.h"



can_message::can_message()
{
  command_data = new unsigned char[8];
  command_data[0] = 0;
  command_data[1] = 0;
  command_data[2] = 0;
  command_data[3] = 0;
  command_data[4] = 0;
  command_data[5] = 0;
  command_data[6] = 0;
  command_data[7] = 0;
}


can_message::~can_message()
{
  delete[] command_data;
}




