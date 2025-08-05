#pragma once

#include "cpu.h"
#include <map>
#include <memory>
#include <mutex>

class CPUManager
{
    int num_of_cpu = 0;
    using index = int;

    std::map<index, std::unique_ptr<CPU>> running_cpus;

public:
    CPUManager();

    void initialize(int num_of_cpu);
    void tick_cpu(int cpu, std::mutex& mutex);
    void tick_active_cpu(int cpu, std::mutex& mutex);
    std::map<index, std::unique_ptr<CPU>>& get_running_cpus();
};

extern CPUManager cpuManager;
