#pragma once

#include "imemory_allocator.h"
#include "process.h"
#include <memory>
#include <vector>

class SchedAlgo 
{
public:
    virtual void add_process(std::shared_ptr<Process> process) = 0;
    //virtual void sort_process_queues() = 0;
    virtual void run_scheduler() = 0;
    virtual std::vector<std::shared_ptr<Process>>& get_running_processes(int core) = 0;
    virtual std::vector<std::shared_ptr<Process>>& get_finished_processes() = 0;
    virtual bool check_if_running() = 0;
    
    virtual int get_num_cores() = 0;
    virtual std::string& get_name() = 0;

    virtual void set_memory_allocator(std::unique_ptr<IMemoryAllocator>&& im) = 0;
    //virtual void execute_process();
};
