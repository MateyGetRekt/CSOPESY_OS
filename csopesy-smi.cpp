#include "commands.h"
#include "screen.h"
#include "screenManager.h"

void csopesy_smi(std::vector<std::string> command_args)
{
    Screen& current_screen = screenManager.getCurrentScreen();
    current_screen.print("Output summary here\n");
}
