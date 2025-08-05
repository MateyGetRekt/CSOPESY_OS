#include "commands.h"
#include "print_texts.h"

#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "screenManager.h"
#include "screen.h"

bool is_initialized = false;

// a function pointer that accepts std::vector<std::string>
typedef void(*func_ptr)(std::vector<std::string>);

bool does_command_exist(
        std::map<std::string, func_ptr>  &command_map,
        std::string                      &input
);
std::map<std::string, func_ptr> prepare_command_args();

ExecutionStatus execute_command()
{
    std::map<std::string, func_ptr> command_map; // a map with key-value pair
                                                 // of a string and a
                                                 // function pointer to
                                                 // the command function
    std::vector<std::string> command_args;
    std::string command_name;
    std::string input;

    command_map = prepare_command_args();
    std::vector<std::string> screen_only_commands({
        "process-smi",
        "exit",
        "help",
        "clear"
    });

    while (true)
    {
        command_args = enter_command();
        command_name = command_args[0];
        Screen& current_screen = screenManager.getCurrentScreen();

        if (screenManager.nameCurrentScreen() != "main") 
        {
            auto iterator = std::find(
                    screen_only_commands.begin(),
                    screen_only_commands.end(),
                    command_name
                    );
            if (iterator == screen_only_commands.end())
            {
                current_screen.print(
                        "only process-smi and exit commands are " 
                        "allowed in generated screen. Use \"exit\" to go back "
                        "to the main screen.\n");
                continue;
            } 

            if (command_name != "exit"
                && command_name != "clear") {
                command_map[command_name](command_args);
                continue;
            }                        
        }
        else
        {
            if (does_command_exist(command_map, command_name))
            {
                if (command_name.compare("initialize") == 0 && !is_initialized) {
                    is_initialized = true;
                    command_map[command_name](command_args);
                    continue;
                }
                else if (command_name.compare("initialize") != 0
                         && !is_initialized
                         && command_name.compare("help") != 0) {
                    current_screen.print("Enter the \'initialize\' command first!\n");
                    continue;
                }
                else if (command_name.compare("initialize") == 0 && is_initialized) {
                    current_screen.print("System already initialized.\n");
                    continue;
                }
                else {
                    command_map[command_name](command_args);
                    continue;
                }
                
            }
        }
        
        
        if (command_name.compare("exit") == 0)
        {
            if (screenManager.nameCurrentScreen() != "main") {
                std::string previous_screen = current_screen.getProcessName();

                current_screen.clear_screen();
                // Move back to main
				screenManager.setCurrentScreen("main");
                Screen& new_screen = screenManager.getScreen("main");
                
                new_screen.restore();

                continue;
            }
			return ExecutionStatus::EXIT;
        }

        if (command_name.compare("clear") == 0)
        {
            current_screen.clear_screen();
            continue;
        }

        std::cout << "Command not found! Type \"help\" "
                  << "to show all commands.\n";
    }
}

bool does_command_exist(
        std::map<std::string, func_ptr>  &command_map,
        std::string                      &input) 
{
    if (command_map.find(input) == command_map.end())
    {
        // didn't find the key
        return false;
    } 
    // it's there
    return true;
}


std::map<std::string, func_ptr> prepare_command_args()
{
    std::map<std::string, func_ptr> command_map;

    command_map["screen"] = screen;
    command_map["initialize"] = initialize;
    command_map["scheduler-test"] = scheduler_test;
    command_map["scheduler-stop"] = scheduler_stop;
    command_map["report-util"] = report_util;
    command_map["process-smi"] = process_smi;
    command_map["csopesy-smi"] = csopesy_smi;
    command_map["add-process-set"] = add_process_set;
    command_map["print"] = print_cpu;
    command_map["help"] = help;
    command_map["vmstat"] = vmstat;

    return command_map;
}
