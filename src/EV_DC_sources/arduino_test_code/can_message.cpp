#include "can_message.h"



can_message::can_message()
{
}


can_message::~can_message()
{
}


int can_message::get_id()
{
    return id;
}

can_message::command_data can_message::get_data()
{
    return data;
}