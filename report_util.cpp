#include "commands.h"
#include "screen.h"
#include "screenManager.h"
#include <fstream>
#include <string>

#define REPORT_FILE "csopesy-log.txt"

void report_util(std::vector<std::string> command_args)
{
    Screen& currentScreen = screenManager.getCurrentScreen();
    std::string output = process_log().str();

    std::ofstream file_stream;
    file_stream.open(REPORT_FILE);
    if (!file_stream.is_open())
    {
        currentScreen.print("Cannot write logs into csopesy-log.txt.\n");
    }
    file_stream << output;
    file_stream.close();
    
    currentScreen.print(process_log().str());
    currentScreen.print("New file created, csopesy-log.txt.\n");
    
}
