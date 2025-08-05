#include <algorithm>
#include <iostream>

#include "fcfssched.h"
#include "scheduler.h"
#include <thread>
#include <vector>
#include <shared_mutex>
#include <condition_variable>
#include <barrier>

std::condition_variable cv_fcfs;
extern std::unique_ptr<std::barrier<>> sync_thread; 
extern std::mutex core_mutex;
/*
private:
    int num_cores;
    std::vector<std::vector<Process>> process_queues; // "One queue for each core"
*/

FCFSScheduler::FCFSScheduler(int cores, Scheduler& _scheduler) 
    : num_cores(cores), 
      scheduler(_scheduler),
      process_queues(scheduler.get_process_queues()), 
      ready_queue(scheduler.get_ready_queue())
{
    process_queues = scheduler.get_process_queues();
    ready_queue = scheduler.get_ready_queue();
    name = "fcfs";
    is_running = false;
    scheduler.set_algorithm(this);
}

FCFSScheduler::FCFSScheduler(Scheduler& scheduler)
    : FCFSScheduler(4, scheduler)
{
    name = "fcfs";
    is_running = false;
}

FCFSScheduler::~FCFSScheduler()
{
}

void FCFSScheduler::set_memory_allocator(std::unique_ptr<IMemoryAllocator>&& im)
{
    memoryAllocator = std::move(im);
}

std::vector<std::shared_ptr<Process>>& FCFSScheduler::get_running_processes(int core) {
    return process_queues[core];
}


std::vector<std::shared_ptr<Process>>& FCFSScheduler::get_finished_processes() {
    return finished_processes;
}

bool FCFSScheduler::check_if_running()
{
    return is_running;
}

std::string& FCFSScheduler::get_name()
{
    return name;
}

void FCFSScheduler::confirm_algorithm(Scheduler& scheduler)
{
    
    num_cores = scheduler.get_num_cores();
    scheduler.set_algorithm(this);
}

void FCFSScheduler::add_process(std::shared_ptr<Process> process) {
    std::unique_lock<std::mutex> coreLock(core_mutex);
    ready_queue.push_back(process);
}

void FCFSScheduler::sort_process_queues()
{
    for (auto& queue : process_queues) {
        std::sort(queue.begin(), queue.end(), [](const std::shared_ptr<Process> a, const std::shared_ptr<Process> b) {
            return a->get_remaining_instructions() > b->get_remaining_instructions();
        });
    }
}

void FCFSScheduler::run_scheduler() {
    std::vector<std::thread> core_threads;

    auto core_worker = [&](int core) {
        int local_delay_count = 0;
        while (true) {
            std::unique_lock<std::mutex> coreLock(core_mutex);
            int delay = sched.get_delays_per_exec();

            if (process_queues[core].empty() && ready_queue.empty()) { // go next
                coreLock.unlock();
                sync_thread->arrive_and_wait();
                coreLock.lock();
                continue;
            }

            if (process_queues[core].empty() && !ready_queue.empty()) { 
                process_queues[core].push_back(ready_queue.front());
                ready_queue.erase(ready_queue.begin());
            }

            std::shared_ptr<Process> currentProcess = process_queues[core].front();
            if (local_delay_count == delay) {
                currentProcess->execute_instruction(core);
                local_delay_count = 0;
            } else local_delay_count++;

            if (currentProcess->has_finished()) {
                finished_processes.push_back(currentProcess);

                coreLock.unlock();
                sync_thread->arrive_and_wait();
                coreLock.lock();
                process_queues[core].erase(process_queues[core].begin());
                continue;
            }

            coreLock.unlock();
            sync_thread->arrive_and_wait();
            coreLock.lock();
        }
    };

    
    for (int core = 0; core < num_cores; ++core) {
        core_threads.emplace_back(core_worker, core);
    }

    for (auto& thread : core_threads) {
        if (thread.joinable()) {
            thread.detach();
        }
    }
}

int FCFSScheduler::get_num_cores()
{
    return num_cores;
}
