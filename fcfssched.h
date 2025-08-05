#pragma once

#include <memory>
#include "process.h"
#include "sched_algo.h"
#include "scheduler.h"
#include <vector>

class FCFSScheduler : SchedAlgo
{
public:
    FCFSScheduler(Scheduler& scheduler);
    ~FCFSScheduler();
    FCFSScheduler(int cores, Scheduler& scheduler);

    void confirm_algorithm(Scheduler& scheduler);
    void add_process(const std::shared_ptr<Process> process);
    void sort_process_queues();
    void run_scheduler();
    std::vector<std::shared_ptr<Process>>& get_running_processes(int core);
    std::vector<std::shared_ptr<Process>>& get_finished_processes();
    bool check_if_running();
    int get_num_cores();
    std::string& get_name();

    void set_memory_allocator(std::unique_ptr<IMemoryAllocator>&& im);

private:
    int num_cores;
    bool is_running;
    std::string name;
    Scheduler& scheduler;

    std::vector<std::vector<std::shared_ptr<Process>>>& process_queues;
    std::vector<std::shared_ptr<Process>>& ready_queue;
    std::vector<std::shared_ptr<Process>> finished_processes;

    std::unique_ptr<IMemoryAllocator> memoryAllocator;
};

extern FCFSScheduler fcfs;
