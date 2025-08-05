#include "print_texts.h"
#include "commands.h"
#include "screenManager.h"
#include "screen.h"

void main_menu()
{
    Screen mainScreen = Screen("main");
    screenManager.addScreen(mainScreen);
    screenManager.setCurrentScreen("main");

    while (true)
    {
        print::header_texts(); 

        if (execute_command() == ExecutionStatus::EXIT) {
            return;
        }

    }
}

