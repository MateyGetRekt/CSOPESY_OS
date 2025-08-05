#pragma once

#include "imemory_allocator.h"
#include "process.h"
#include "sched_algo.h"
#include "scheduler.h"
#include <vector>
#include <memory>

class RRScheduler : SchedAlgo
{
public:
    RRScheduler(Scheduler& scheduler);
    RRScheduler(int cores, Scheduler& scheduler);

    ~RRScheduler();

    void confirm_algorithm(Scheduler& scheduler);
    void add_process(std::shared_ptr<Process> process);
    void run_scheduler();
    bool check_if_running();

    std::vector<std::shared_ptr<Process>>& get_running_processes(int core);
    std::vector<std::shared_ptr<Process>>& get_finished_processes();
    int get_num_cores();
    std::string& get_name();

    void set_memory_allocator(std::unique_ptr<IMemoryAllocator>&& im);

private:
    int num_cores;
    bool is_running;
    std::string name;

    // The real process_queues is in the Scheduler class,
    // thus it is a reference for now.
    std::vector<std::vector<std::shared_ptr<Process>>>& process_queues;
    std::vector<std::shared_ptr<Process>>& ready_queue;
    std::vector<std::shared_ptr<Process>> finished_processes;

    std::unique_ptr<IMemoryAllocator> memory_allocator;
};

extern RRScheduler rr;
