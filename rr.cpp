#include "rr.h"
#include "cpu_manager.h"
#include "imemory_allocator.h"
#include "scheduler.h"
#include "screenManager.h"
#include "memory-manager.h"
#include <format>
#include <memory>
#include <thread>
#include <utility>
#include <vector>
#include <condition_variable>
#include <iostream>
#include <barrier>

extern std::mutex core_mutex;
std::condition_variable cv_rr;
extern std::unique_ptr<std::barrier<>> sync_thread;

RRScheduler::RRScheduler(Scheduler& scheduler)
    : RRScheduler(4, scheduler)
{
    name = "rr";
    is_running = false;
}

RRScheduler::~RRScheduler() {

}

void RRScheduler::set_memory_allocator(std::unique_ptr<IMemoryAllocator>&& im)
{
    this->memory_allocator = std::move(im);
}

RRScheduler::RRScheduler(int cores, Scheduler& scheduler)
	: num_cores(cores), 
      name("rr"),
      process_queues(scheduler.get_process_queues()), 
      ready_queue(scheduler.get_ready_queue())
{
	process_queues = scheduler.get_process_queues();
    ready_queue = scheduler.get_ready_queue();
	scheduler.set_algorithm(this);
}

void RRScheduler::confirm_algorithm(Scheduler& scheduler) {
	num_cores = scheduler.get_num_cores();
	scheduler.set_algorithm(this);
}


void RRScheduler::add_process(const std::shared_ptr<Process> process) {
    std::lock_guard<std::mutex> lock(core_mutex);
    ready_queue.push_back(process);
}

void RRScheduler::run_scheduler() {
    std::vector<std::thread> core_threads;
    cpuManager.initialize(num_cores);

    auto core_worker = [&](int core) {
        int local_cycles = 0;
        int local_delay_count = 0;
        while (true) {
            int time_slice = sched.get_quantum_cycles();
            int delay = sched.get_delays_per_exec();
            //std::cout << "within loop value: " << time_slice << "\n";
            std::unique_lock<std::mutex> coreLock(core_mutex);

            // if there is no work to be done
            if (process_queues[core].empty() && ready_queue.empty()) {
                coreLock.unlock();
                sync_thread->arrive_and_wait();
                coreLock.lock();
                continue;
            }
            // move next in rq to core if empty
            else if (process_queues[core].empty() && !ready_queue.empty()) {
                process_queues[core].push_back(ready_queue.front());
                ready_queue.erase(ready_queue.begin());
            }
            
            std::shared_ptr<Process> currentProcess = process_queues[core].front();

            //std::cout << "need to allocate on core " << core << ": " << !currentProcess->get_is_allocated() << "\n";
            //std::cout << "cant allocate on core " << core << ": " << !memory_allocator->can_allocate(currentProcess->get_memory_required()) << "\n";
            // see if allocation needed and if cant allocate
            if (!memory_allocator->can_allocate(currentProcess->get_memory_required()) &&
                !currentProcess->get_is_allocated()) {
                std::cout << "go next fast in core " << core << " :(\n";
                ready_queue.push_back(currentProcess);

                process_queues[core].erase(process_queues[core].begin());
                coreLock.unlock();
                sync_thread->arrive_and_wait();
                coreLock.lock();
                
                continue;
            }

            coreLock.unlock();
            // allocate memory if not already allocated
            if (!currentProcess->get_is_allocated()) {
                void* memory = memory_allocator->allocate(currentProcess->get_memory_required(), static_cast<size_t>(currentProcess->get_process_id()));

                if (memory == nullptr) {
                    //std::cout << "cant allocate in core " << core << " :(\n";
                    sync_thread->arrive_and_wait();
                    coreLock.lock();
                    continue;
                }

                memory_allocator->add_to_process_tracking(memory, currentProcess->get_process_name());
                currentProcess->set_is_allocated(true);
                currentProcess->set_memory_ptr(memory);
            }

            // run once if cpu has waited long enough else keep waiting
            if (local_delay_count == delay) {
                currentProcess->execute_instruction(core);
                cpuManager.tick_active_cpu(core, core_mutex);
                //std::cout << "executed once\n";
                local_delay_count = 0;
            } else {
                local_delay_count++;
            }

            cpuManager.tick_cpu(core, core_mutex);

            coreLock.lock();
            // post-execution handling
            if (currentProcess->has_finished()) {
                local_cycles = 0;
                memory_allocator->visual_memory();

                finished_processes.push_back(currentProcess);
                memory_allocator->deallocate(currentProcess->get_memory_ptr(), currentProcess->get_memory_required(), static_cast<size_t>(currentProcess->get_process_id()));
                memory_allocator->delete_from_process_tracking(currentProcess->get_memory_ptr(), currentProcess->get_process_name());

                process_queues[core].erase(process_queues[core].begin());
                coreLock.unlock();
                sync_thread->arrive_and_wait();
                coreLock.lock();
                
                continue;
            }

            if (local_cycles == time_slice) {
                //std::cout << "timeslice is up in core " << core << "\n";
                local_cycles = 0;
                memory_allocator->visual_memory();
                //std::cout << "Memory Visualization\n";
                ready_queue.push_back(currentProcess);

                process_queues[core].erase(process_queues[core].begin());
                coreLock.unlock();
                sync_thread->arrive_and_wait();
                coreLock.lock();
                
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

std::vector<std::shared_ptr<Process>>& RRScheduler::get_running_processes(int core) {
	return process_queues[core];
}

std::vector<std::shared_ptr<Process>>& RRScheduler::get_finished_processes() {
	return finished_processes;
}

bool RRScheduler::check_if_running()
{
	return is_running;
}

int RRScheduler::get_num_cores()
{
	return num_cores;
}

std::string& RRScheduler::get_name()
{
    return name;
}
