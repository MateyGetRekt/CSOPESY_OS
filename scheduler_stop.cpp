#include "commands.h"
#include <iostream>

void scheduler_stop(std::vector<std::string> command_args)
{
    std::lock_guard<std::mutex> lock(stop_flag_mutex);
    stop_flag = true;  
}
