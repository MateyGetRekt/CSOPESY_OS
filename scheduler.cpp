#include "scheduler.h"
#include "commands.h"
#include "sched_algo.h"
#include "screenManager.h"
#include <sstream>
#include <vector>
#include <iostream>

Scheduler::Scheduler(){
};

Scheduler::~Scheduler() = default;

void Scheduler::add_process_set(int num_process)
{   

}

void Scheduler::scheduler_test()
{
    algo->run_scheduler();
}

void Scheduler::scheduler_stop()
{

}

void Scheduler::set_algorithm(SchedAlgo* _algo) {
    algo = _algo;
}

int Scheduler::get_cpu_utilization() 
{
    /*  
        int num_cores;
        int quantum_cycles;
        int batch_process_freq;
        int delays_per_exec;
     * */
    int max_num_cores = num_cores;
    int num_running_cores = get_num_running_cores();

    float cpu_utilitzation = 100.f * static_cast<float>(num_running_cores) / static_cast<float>(max_num_cores);
    return cpu_utilitzation;
}


int Scheduler::get_num_running_cores()
{
    int num_running_cores = 0;

    for (const auto& a : process_queues)
    {
        if (!a.empty())
        {
            num_running_cores++;
        }
    }
    return num_running_cores;
}

int Scheduler::get_num_available_cores()
{
    int num_available_cores = 0;

    for (const auto& a : process_queues)
    {
        if (a.empty())
        {
            num_available_cores++;
        }
    }
    return num_available_cores;

}

SchedAlgo* Scheduler::get_algorithm()
{
    if (algo == nullptr) {
        throw std::runtime_error("Algorithm not set");
    }
    return algo;
}

void Scheduler::generate_cpu_queues()
{
    Screen& currentScreen = screenManager.getCurrentScreen();

    for (int i = 0; i < num_cores; i++) {
        std::vector<std::shared_ptr<Process>> core_queue;
        process_queues.push_back(core_queue);
    }

    std::stringstream ss;
    ss << "Generated Core Queues: " << process_queues.size() << "\n";
    currentScreen.print(ss.str());
}

void Scheduler::set_num_cores(int num_cores)
{
    this->num_cores = num_cores;
}

void Scheduler::set_quantum_cycles(int quantum_cycles)
{
    this->quantum_cycles = quantum_cycles;
}

void Scheduler::set_batch_process_freq(int batch_process_freq)
{
    this->batch_process_freq = batch_process_freq;
}

void Scheduler::set_delays_per_exec(int delays_per_exec)
{
    this->delays_per_exec = delays_per_exec;
}

int Scheduler::get_num_cores()
{
    return num_cores;
}

int Scheduler::get_delays_per_exec()
{
    return delays_per_exec;
}

int  Scheduler::get_batch_freq()
{
    return batch_process_freq;
}

std::vector<std::vector<std::shared_ptr<Process>>>& Scheduler::get_process_queues()
{
    return process_queues;
}

std::vector<std::shared_ptr<Process>>& Scheduler::get_ready_queue()
{
    return ready_queue;
}

int Scheduler::get_quantum_cycles()
{
    return quantum_cycles;
}

SchedAlgo* Scheduler::get_algo()
{
    return algo;
}
