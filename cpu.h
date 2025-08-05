#pragma once

#include <mutex>
class CPU
{
    int tick = 0;
    int active_tick = 0;
    bool active = false;

public:
    CPU() = default;
    int add_tick();
    int get_tick();

    int add_active_tick();
    int get_active_tick();

    void activate(std::mutex mutex);
    void deactivate(std::mutex mutex);
    bool is_active();
};
