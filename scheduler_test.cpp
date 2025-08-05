#include "commands.h"
#include "screenManager.h"
#include "scheduler.h"
#include <string>
#include <iostream>
#include <thread>
#include <atomic>
#include <barrier>

std::atomic<bool> stop_flag(true);  
std::thread scheduler_thread;
extern std::unique_ptr<std::barrier<>> sync_thread;
std::mutex stop_flag_mutex;

void scheduler_test(std::vector<std::string> command_args) {
    if (!stop_flag) {
        std::cout << "scheduler is already running." << std::endl;
        return;
    }

    Screen& currentScreen = screenManager.getCurrentScreen();
    if (currentScreen.getProcessName() != "main") {
        currentScreen.print("Undefined behavior?");
        return;
    }
    
    std::lock_guard<std::mutex> lock(stop_flag_mutex);
    stop_flag = false; 
}

void scheduler_thread_start() {
    scheduler_thread = std::thread([&]() {
        int cycle_counter = 0;
        while(true){
            SchedAlgo* currentAlgo = sched.get_algorithm();
            int batch_freq = sched.get_batch_freq();
            if (!stop_flag) {  
                while (cycle_counter % batch_freq != 0) {
                    cycle_counter++;
                    sync_thread->arrive_and_wait();
                    continue;
                }
                std::map<std::string, Screen>& screensMap = screenManager.getScreensMap();
                int numScreens = static_cast<int>(screensMap.size());
                int buffer = 0;

                std::string process_name = "p" + std::to_string(numScreens);

                while (screensMap.find(process_name) != screensMap.end()) {
                    process_name = "p" + std::to_string(numScreens + buffer);
                    buffer++;
                }

                Screen new_screen(process_name);
                screenManager.addScreen(new_screen);
                screenManager.getScreen(process_name).info_log();
                currentAlgo->add_process(new_screen.get_process());

            }
            
            //std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            cycle_counter++;
            sync_thread->arrive_and_wait();
        }
    });
}

