#include "display_memory.h"
#include "../memory-manager.h"
#include <iostream>
#include <format>

// Shows total, used, and free memory in KiB
void display_memory()
{
    int used_memory = memManager.get_memory_allocated_size();
    int total_memory = memManager.get_max_overall_mem();
    int free_memory = total_memory - used_memory;

    std::cout << std::format("{:13} K total memory", total_memory) << "\n";
    std::cout << std::format("{:13} K used memory", used_memory) << "\n";
    std::cout << std::format("{:13} K free memory", free_memory) << "\n";
}