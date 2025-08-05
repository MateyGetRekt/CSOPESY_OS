#pragma once
#include <memory>
#include "process.h"
#include "sched_algo.h"
#include <vector>
class Scheduler
{
public:
    Scheduler();
    ~Scheduler();

    void add_process_set(int num_process);
    void scheduler_test();
    void scheduler_stop();
    void set_algorithm(SchedAlgo* _algo);
    void generate_cpu_queues();
    
    void set_num_cores(int num_cores);
    void set_quantum_cycles(int quantum_cycles);
    void set_batch_process_freq(int batch_process_freq);
    void set_delays_per_exec(int delays_per_exec);

    int get_quantum_cycles();
    SchedAlgo* get_algorithm();
    SchedAlgo* get_algo();
    int get_num_cores();
    int get_delays_per_exec();
    int get_batch_freq();
    int get_cpu_utilization();
    int get_num_running_cores();
    int get_num_available_cores();

    std::vector<std::vector<std::shared_ptr<Process>>>& get_process_queues();
    std::vector<std::shared_ptr<Process>>& get_ready_queue();

private:
    std::vector<std::vector<std::shared_ptr<Process>>> process_queues;
    std::vector<std::shared_ptr<Process>> ready_queue;

    int num_cores;
    int quantum_cycles;
    int batch_process_freq;
    int delays_per_exec;

    SchedAlgo* algo = nullptr;
};

extern Scheduler sched;
