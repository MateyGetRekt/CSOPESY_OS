#include "commands.h"
#include "vmstat/display_cpu_ticks.h"
#include "vmstat/display_memory.h"
#include "vmstat/display_pages_inout.h"

void vmstat(std::vector<std::string> command_args)
{
    display_memory();
    display_cpu_ticks();
    display_pages_inout();
}
