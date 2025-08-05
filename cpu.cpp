#include "cpu.h"
#include <mutex>

int CPU::add_tick()
{
    // IMPORTANT: only uses prefix-increment.
    //            It returns a variable that is incremented.
    //            postfix (tick++) returns the tick BEFORE it
    //            is incremented
    return ++tick;
}

int CPU::get_tick()
{
    return tick;
}

int CPU::add_active_tick()
{
    return ++active_tick;
}

int CPU::get_active_tick()
{
    return active_tick;
}

void CPU::activate(std::mutex mutex)
{
    std::unique_lock<std::mutex> lock(mutex);
    active = true;
}

void CPU::deactivate(std::mutex mutex)
{
    std::unique_lock<std::mutex> lock(mutex);
    active = false;
}

bool CPU::is_active()
{
    return active;
}
