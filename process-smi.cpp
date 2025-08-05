#include "commands.h"
#include "screenManager.h"
#include "memory-manager.h"
#include "sched_algo.h"
#include "scheduler.h"

extern Scheduler sched;
extern MemoryManager memManager;

void process_smi(std::vector<std::string> command_args)
{
    Screen& currentScreen = screenManager.getCurrentScreen();

    if (currentScreen.getProcessName() == "main") {
        SchedAlgo* currentAlgo = sched.get_algorithm();
        int cpu_util = sched.get_cpu_utilization();
        int allocated_memory = memManager.get_memory_allocated_size();
        int total_memory = memManager.get_memory_max_size();
        int mem_util = memManager.get_mem_util();

        currentScreen.print("\n------------------------------------------------------------------------------------");
        currentScreen.print("\n|                    PROCESS-SMI V01.00 Driver Version: 01.00                      |");
        currentScreen.print("\n------------------------------------------------------------------------------------\n");
        currentScreen.print("CPU-Util: " + std::to_string(cpu_util) + "%\n");
        currentScreen.print("Memory Usage: " + std::to_string(allocated_memory) + " KiB / " + std::to_string(total_memory) + " KiB" + "\n");
        currentScreen.print("Memory Util: " + std::to_string(mem_util) + "%\n");
        currentScreen.print("\n====================================================================================\n");
        currentScreen.print("Running processes and memory usage:                                                   \n");
        currentScreen.print("------------------------------------------------------------------------------------\n");
        for (int core = 0; core < sched.get_num_cores(); core++)
        {
            std::vector<std::shared_ptr<Process>> running_processes;
            running_processes = sched.get_process_queues()[core];

            for (auto& process : running_processes)
            {
                std::string process_name = process->get_process_name();
                int mem_required = process->get_memory_required();
                currentScreen.print(process_name + " " + std::to_string(mem_required) +  " KiB" + "\n");
            }
        }
        currentScreen.print("------------------------------------------------------------------------------------\n");



    }
    else {
        currentScreen.print("\n");
        currentScreen.info();
    }
}
