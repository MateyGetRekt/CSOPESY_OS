#include "commands.h"
#include "sched_algo.h"
#include "scheduler.h"
#include "screenManager.h"
#include "screen.h"
#include "process.h"

#include <chrono>
#include <cstddef>
#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>
#include <map>
#include <format>
#include <fstream>
#include <filesystem>
#include <mutex>
#include <cstdlib>
#include <ctime>

#include "fcfssched.h"
#include "rr.h"
#include "memory-manager.h"

extern FCFSScheduler fcfs;
extern RRScheduler rr;
extern MemoryManager memManager;
extern std::mutex core_mutex;
extern std::mutex memory_mutex;

int Screen::max_process_id = 0;

std::stringstream process_log()
{
    std::stringstream output;
    std::unique_lock<std::mutex> coreLock(core_mutex);
    auto& screensMap = screenManager.getScreensMap();
    SchedAlgo* currentAlgo = sched.get_algorithm();

    // CPU utilization summary
    output << std::format("CPU utilization: {}%\n", sched.get_cpu_utilization());
    output << std::format("Cores used: {}\n", sched.get_num_running_cores());
    output << std::format("Cores available: {}\n", sched.get_num_available_cores());

    // Ready queue
    auto ready_queue = sched.get_ready_queue();
    size_t rq_start = (ready_queue.size() > 10 ? ready_queue.size() - 10 : 0);
    if (ready_queue.empty()) {
        output << "--------------------------------------------------------------------------------------\n"
            << "No Processes in Ready Queue\n";
    }
    else {
        output << "Ready Queue\n";
        for (size_t i = rq_start; i < ready_queue.size(); ++i) {
            auto& proc = ready_queue[i];
            std::string name = proc->get_process_name();
            Screen& screen = screensMap[name];
            output << name
                << " (" << proc->get_process_id() << ")\t"
                << screen.get_timestamp() << "\t"
                << " Waiting \t"
                << proc->get_total_instructions() - proc->get_remaining_instructions()
                << " / " << proc->get_total_instructions() << "\n";
        }
    }

    // Running processes
    output << "\nRunning processes\n";
    for (int core = 0; core < sched.get_num_cores(); ++core) {
        auto running = sched.get_process_queues()[core];
        for (auto& proc : running) {
            std::string name = proc->get_process_name();
            Screen& screen = screensMap[name];
            output << name
                << " (" << proc->get_process_id() << ")\t"
                << screen.get_timestamp() << "\t"
                << " Core: " << core + 1 << "\t"
                << proc->get_total_instructions() - proc->get_remaining_instructions()
                << " / " << proc->get_total_instructions() << "\n";
        }
    }

    // Finished processes
    auto& finished = currentAlgo->get_finished_processes();
    if (finished.empty()) {
        output << "--------------------------------------------------------------------------------------\n"
            << "No Finished Processes\n";
        return output;
    }
    output << "\nFinished processes (limited to 20)\n";
    size_t fi_start = (finished.size() > 10 ? finished.size() - 10 : 0);
    for (size_t i = fi_start; i < finished.size(); ++i) {
        auto& proc = finished[i];
        std::string name = proc->get_process_name();
        Screen& screen = screensMap[name];
        output << name
            << " (" << proc->get_process_id() << ")\t"
            << screen.get_timestamp() << "\t"
            << " Finished\t"
            << proc->get_total_instructions() - proc->get_remaining_instructions()
            << " / " << proc->get_total_instructions() << "\n";
    }
    output << "--------------------------------------------------------------------------------------\n";

    return output;
}

std::string formatProcessName(const std::string& process_name, size_t space_count) {
    if (process_name.length() <= space_count) {
        return process_name + std::string(space_count - process_name.length(), ' ');
    }
    else {
        return "..." + process_name.substr(process_name.length() - (space_count - 3));
    }
}

void screen(std::vector<std::string> commands)
{
    Screen& currentScreen = screenManager.getCurrentScreen();
    SchedAlgo* currentAlgo = sched.get_algorithm();

    if (commands.size() == 1) {
        currentScreen.print("Invalid command format. Usage: screen -r <process_name>\n");
        return;
    }

    std::string option = commands[1];
    std::string process_name = (commands.size() > 2 ? commands[2] : "");

    if (option == "-s") {
        auto& screensMap = screenManager.getScreensMap();
        if (screensMap.find(process_name) != screensMap.end()) {
            currentScreen.print("Screen for process " + process_name + " already exists\n");
            return;
        }

        // 1) Parse optional memory-size argument (default = mem-per-proc)
        size_t req_size = memManager.get_mem_per_proc();
        if (commands.size() > 3) {
            req_size = std::stoul(commands[3]);
        }

        // 2) Validate: must be power-of-two between 64 and 65536
        if ((req_size & (req_size - 1)) != 0 || req_size < 64 || req_size > 65536) {
            currentScreen.print(
                "Error: memory size must be a power of two between 64 and 65536.\n"
            );
            return;
        }

        clear();
        std::cout << "1";

        // 3) Create screen with explicit memory requirement
        Screen new_screen(process_name, req_size);
        std::cout << "2";

        screenManager.addScreen(new_screen);
        std::cout << "3";

        screenManager.setCurrentScreen(process_name);
        std::cout << "4";

        screenManager.getScreen(process_name).info();
        std::cout << "5";

        // 4) Queue the process for execution
        currentAlgo->add_process(new_screen.get_process());
        std::cout << "6";

        // ── NEW: Charge memory here, or reject if cap exceeded ───────────────────
        {
            size_t used = memManager.get_memory_allocated_size();
            size_t cap = memManager.get_max_overall_mem();
            if (used + req_size > cap) {
                currentScreen.print(
                    "Error: Not enough memory to allocate " + process_name + "\n"
                );
                // Optionally: remove the screen you just added
                screenManager.removeScreen(process_name);
                return;
            }
            memManager.add_memory(req_size);
        }
    }

    else if (option == "-r") {
        auto& screensMap = screenManager.getScreensMap();
        if (screensMap.find(process_name) != screensMap.end() &&
            !screenManager.getScreen(process_name).get_process()->has_finished()) {
            clear();
            screenManager.setCurrentScreen(process_name);
            screensMap[process_name].restore();
        }
        else {
            currentScreen.print("Process " + process_name + " not found.\n");
        }
    }
    else if (option == "-ls") {
        currentScreen.print(process_log().str());
    }
    else {
        currentScreen.print("Unknown Command. Use -s to create, "
            "-r to restore a screen or -ls to list all screen.\n");
    }
}

Screen::Screen() : process_name("Unnamed"), current_instruction("")
{
    int mem_req = memManager.get_mem_per_proc();
    int num_pages = mem_req / memManager.get_mem_per_frame();
    process = std::make_shared<Process>("Unnamed", 0, 0, mem_req, num_pages);
    timestamp = get_current_time();
}

// New ctor: specify memory requirement
Screen::Screen(const std::string& name, size_t mem_required)
    : process_name(name),
    current_instruction("")
{
    if (name != "main") {
        int num_pages = int(
            (mem_required + memManager.get_mem_per_frame() - 1)
            / memManager.get_mem_per_frame()
            );
        process = std::make_shared<Process>(
            name,
            max_process_id,
            randomize_num_instructions(),
            int(mem_required),
            num_pages
        );
    }
    max_process_id++;
    timestamp = get_current_time();
}

Screen::Screen(const std::string& name)
    : process_name(name),
    current_instruction("")
{
    if (name != "main") {
        int mem_req = memManager.get_mem_per_proc();
        int num_pages = (mem_req + memManager.get_mem_per_frame() - 1)
            / memManager.get_mem_per_frame();
        process = std::make_shared<Process>(
            name,
            max_process_id,
            randomize_num_instructions(),
            mem_req,
            num_pages
        );
    }
    max_process_id++;
    timestamp = get_current_time();
}

void Screen::info() {
    print("Process: " + process_name + "\n");
    print("ID: " + std::to_string(process->get_process_id()) + "\n");
    if (process->get_remaining_instructions() != 0) {
        print("\nCurrent instruction line: " +
            std::to_string(process->get_total_instructions() -
                process->get_remaining_instructions()) + "\n");
        print("Lines of code: " +
            std::to_string(process->get_total_instructions()) + "\n");
    }
    else {
        print("\nFinished!\n");
    }
}

void Screen::print(std::string&& text) {
    std::cout << text;
    captureLog(text);
}

void Screen::info_log() {
    captureLog("Process: " + process_name + "\n");
    captureLog("ID: " + std::to_string(process->get_process_id()) + "\n");
    if (process->get_remaining_instructions() != 0) {
        captureLog("\nCurrent instruction line: " +
            std::to_string(process->get_total_instructions() -
                process->get_remaining_instructions()) +
            "\n");
        captureLog("Lines of code: " +
            std::to_string(process->get_total_instructions()) + "\n");
    }
    else {
        captureLog("\nFinished!\n");
    }
}

void Screen::restore() {
    for (auto& log : logs) {
        std::cout << log;
    }
    print("\n---\nRestoring screen for process: " + process_name + "\n");
}

void Screen::captureLog(std::string log) {
    logs.push_back(log);
}

std::shared_ptr<Process> Screen::get_process() {
    return process;
}

void Screen::clear_screen() {
    clear();
}

std::string Screen::getProcessName() {
    return process_name;
}

int Screen::randomize_num_instructions() {
    int min = screenManager.get_min_ins();
    int max = screenManager.get_max_ins();
    return min + (rand() % (max - min + 1));
}

std::string Screen::get_current_time() {
    auto local_t = std::chrono::current_zone()
        ->to_local(std::chrono::system_clock::now());
    return std::format("{:%m/%d/%Y, %I:%M:%S %p}", local_t);
}

std::string Screen::get_timestamp() {
    size_t comma = timestamp.find(',');
    std::string date = timestamp.substr(0, comma);
    std::string t_ampm = timestamp.substr(comma + 2);
    size_t dot = t_ampm.find('.');
    std::string time = t_ampm.substr(0, dot);
    std::string ampm = t_ampm.substr(t_ampm.size() - 2);
    std::stringstream ss;
    ss << "(" << date << " " << time << ampm << ")";
    return ss.str();
}
