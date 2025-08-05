#include "fcfssched.h"
#include "text.h"
#include "print_texts.h"
#include <chrono>
#include <format>
#include "screenManager.h"
#include "screen.h"

void print::header_texts()
{
    Screen& currentScreen = screenManager.getCurrentScreen();

    currentScreen.print(text::header);

    currentScreen.print(text::welcome);

    currentScreen.print(text::instruction);
}

void print::date_time()
{
    Screen& currentScreen = screenManager.getCurrentScreen();
    auto const local_time = std::chrono::current_zone()
	 				->to_local(std::chrono::system_clock::now());
    
	currentScreen.print(std::format("{:%m/%d/%Y, %I:%M:%S %p}", local_time));

}

void print::cpu_tables()
{
    
}
