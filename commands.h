#pragma once

#include <string>
#include <vector>
#include <atomic>
#include <thread>
#include <mutex>

typedef enum 
{
    SUCCESS,
    EXIT
} ExecutionStatus;

void            main_menu      ();
ExecutionStatus execute_command();
void            initialize     (std::vector<std::string> command_args);
void            screen         (std::vector<std::string> command_args);
void            scheduler_test (std::vector<std::string> command_args);
void            scheduler_stop (std::vector<std::string> command_args);
void            report_util    (std::vector<std::string> command_args);
void            add_process_set(std::vector<std::string> command_args);
void            process_smi    (std::vector<std::string> command_args);
void            csopesy_smi    (std::vector<std::string> command_args);
void            print_cpu      (std::vector<std::string> command_args);
void            cpu_util       (std::vector<std::string> command_args);
void            vmstat         (std::vector<std::string> command_args);
void            scheduler_thread_start();

void clear();
void help(std::vector<std::string> command_args);

std::vector<std::string> enter_command();

extern std::atomic<bool> stop_flag; 
extern std::thread scheduler_thread;
extern std::mutex stop_flag_mutex;
