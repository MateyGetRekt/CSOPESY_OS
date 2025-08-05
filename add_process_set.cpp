#include "commands.h"
#include "scheduler.h"
#include <sstream>

void get_num(std::string& arg, int& out);


void add_process_set(std::vector<std::string> command_args)
{
    int num_process = 10;
    if (command_args.size() > 1)
        get_num(command_args[1], num_process);

    sched.add_process_set(num_process);
}

void get_num(std::string& arg, int& out)
{
    std::stringstream ss(arg);
    ss >> out;
}
