#include "commands.h"
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include "screenManager.h"
#include "screen.h"

std::vector<std::string> str_split(std::string text, char delimiter);
bool check_empty(std::string& input);

std::vector<std::string> enter_command()
{
    std::string input;
    std::vector<std::string> list;

    Screen& currentScreen = screenManager.getCurrentScreen();

    while (true) {
        currentScreen.print("Enter a command: ");
        std::getline(std::cin, input);
		currentScreen.captureLog(input + "\n");
        
        if (check_empty(input)) {
            currentScreen.print("Input is empty.\n");
            continue;
        }
        break;
    }

	list = str_split(input, ' ');
    return list;
}

std::vector<std::string> str_split(std::string text, char delimiter)
{
    std::vector<std::string> list;
    std::stringstream input_stream;
    std::string substring;

    input_stream.str(text);

    while (std::getline(input_stream, substring, delimiter)) {
        list.push_back(substring);
    }
    return list;
}


bool check_empty(std::string& input)
{
    return input.size() == 0 || input == "";
}
