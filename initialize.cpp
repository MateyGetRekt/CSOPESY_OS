#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <barrier>

#include "commands.h"
#include "memory-manager.h"
#include "screenManager.h"
#include "screen.h"
#include "fcfssched.h"
#include "rr.h"
#include "scheduler.h"
#include "flat_memory_allocator.h"
#include "cpu_manager.h"
#include "commands.h"

ScreenManager screenManager;
MemoryManager memManager;
CPUManager cpuManager;
Scheduler sched;
FCFSScheduler fcfs(sched);
RRScheduler rr(sched);
std::unique_ptr<std::barrier<>> sync_thread;
std::mutex core_mutex;
std::mutex memory_mutex;


std::map<std::string, std::string> read_config_file() {
    Screen& currentScreen = screenManager.getCurrentScreen();

    std::ifstream file("config.txt");
    std::map<std::string, std::string> params;
    std::string param;

    if (!file) {
        // std::cerr << "Error: config.txt not found." << std::endl;
        currentScreen.print("config.txt not found.");
        exit(1);
        return params;
    }
    
    while (std::getline(file, param)) {
        std::istringstream iss(param);

        std::string paramName, paramValue;

        iss >> paramName >> paramValue;

        if (paramValue == "") {
            currentScreen.print("Error " + paramName + " is missing a value."
                    + " Check the config.txt.");
            exit(1);
            return params;
        }
        params[paramName] = paramValue;
    }

    file.close();
    return params;
}

void initialize(std::vector<std::string> command_args)
{
    Screen& currentScreen = screenManager.getCurrentScreen();
    currentScreen.print("Initializing\n");

    // TODO: Distribute params to scheduler

    std::map<std::string, std::string> params = read_config_file();
    int numCores = std::stoi(params["num-cpu"]);
    sched.set_num_cores(numCores);
    sync_thread = std::make_unique<std::barrier<>>(numCores + 1);
    //sync_thread = std::make_unique<std::barrier<>>(
    //    numCores + 1, 
    //    []() {
    //        std::cout << "All threads have reached the barrier. Continuing...\n";
    //        // possibly have count for total cpu cycles here??
    //    }
    //);
    sched.generate_cpu_queues();

    if (params["scheduler"] == "\"fcfs\"") {
        fcfs.confirm_algorithm(sched);
        currentScreen.print("FCFS confirmed\n");
    }
    else if (params["scheduler"] == "\"rr\"") {
        rr.confirm_algorithm(sched);
        currentScreen.print("Round Robin confirmed\n");
    }

    // TODO: (low prio) error handling for incorrect params
    else {
        return;
    }

    sched.set_quantum_cycles(std::stoi(params["quantum-cycles"]));
    sched.set_batch_process_freq(std::stoi(params["batch-process-freq"]));
    screenManager.set_min_ins(std::stoi(params["min-ins"]));
    screenManager.set_max_ins(std::stoi(params["max-ins"]));
    sched.set_delays_per_exec(std::stoi(params["delays-per-exec"]));

    sched.scheduler_test();

    scheduler_thread_start();
    memManager.initialize(params);
}


