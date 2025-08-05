#include "cpu_manager.h"
#include "cpu.h"
#include <memory>
#include <mutex>

CPUManager::CPUManager()
    : num_of_cpu{0}
{}

void CPUManager::initialize(int num_of_cpu)
{
    for (int i = 0; i < num_of_cpu; i++)
    {
        running_cpus[i] = std::make_unique<CPU>();
        this->num_of_cpu++;
    }
}

void CPUManager::tick_cpu(int cpu, std::mutex& mutex)
{
    std::unique_lock<std::mutex> lock(mutex);
    running_cpus[cpu]->add_tick();
}

void CPUManager::tick_active_cpu(int cpu, std::mutex& mutex)
{
    std::unique_lock<std::mutex> lock(mutex);
    running_cpus[cpu]->add_active_tick();
}

std::map<CPUManager::index, std::unique_ptr<CPU>>& CPUManager::get_running_cpus()
{
    return running_cpus;
}

