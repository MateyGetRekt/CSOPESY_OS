#include "commands.h"
#include "screenManager.h"
#include "text.h"
#include <cstdio>
#include <cstdlib>
#include <fstream>

#define HELP_TXT "help.txt"
#define SCREEN_HELP_TXT "screen-help.txt"

void help(std::vector<std::string> command_args)
{
    Screen& currentScreen = screenManager.getCurrentScreen();

    std::ifstream ifs;
    
    if (currentScreen.getProcessName().compare("main") == 0)
    {
        ifs.open(HELP_TXT);
        if (!ifs.is_open())
        {
            currentScreen.print("ERROR " + std::string(text::error::no_file::help_txt));
            return;
        }

        std::string line;
        while (getline(ifs, line))
        {
            currentScreen.print(line + "\n");
        }
    }
    else
    {
        ifs.open(SCREEN_HELP_TXT);
        if (!ifs.is_open())
        {
            currentScreen.print("ERROR " + std::string(text::error::no_file::screen_help_txt));
            return;
        }

        std::string line;
        while (getline(ifs, line))
        {
            currentScreen.print(line + "\n");
        }
    }

    ifs.close();
}
