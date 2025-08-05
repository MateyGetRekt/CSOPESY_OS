#pragma once

#include <memory>
#include <string>
#include <vector>
#include <ctime>
#include <sstream>

#include "process.h"

std::stringstream process_log();

class Screen {
public:
    Screen();
    Screen(const std::string& name);
    Screen(const std::string& name, size_t mem_required);  // specify process memory
    void print(std::string&& text);
    void info();
    void info_log();
    void restore();
    void clear_screen();
    std::string getProcessName();
    void captureLog(std::string log);
    std::shared_ptr<Process> get_process();
    std::string get_timestamp();

private:
    std::string process_name;
    std::string current_instruction;
    std::string timestamp;
    std::vector<std::string> logs;
    static int max_process_id;
    std::shared_ptr<Process> process;

    int randomize_num_instructions();
    std::string get_current_time();
};
