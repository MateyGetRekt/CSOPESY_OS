#include "display_cpu_ticks.h"
#include "../cpu_manager.h"

#include <format>
#include <iostream>

void display_cpu_ticks()
{
    int idle_cpu_ticks = 0;
    int active_cpu_ticks = 0;
    int total_cpu_ticks = 0;

    for (auto const& [index, cpu]: cpuManager.get_running_cpus())
    {
        active_cpu_ticks = cpu->get_active_tick();
        total_cpu_ticks = cpu->get_tick();
    }

    idle_cpu_ticks = total_cpu_ticks - active_cpu_ticks;

    // Detailed per-CPU ticks 
    for (auto const& [id, cpu] : cpuManager.get_running_cpus()) {
        int a = cpu->get_active_tick();
        int t = cpu->get_tick();
        int i = t - a;
        std::cout << std::format(
            "CPU {:<2} idle: {:<6} active: {:<6} total: {}\n",
            id, i, a, t
        );
    }
}
