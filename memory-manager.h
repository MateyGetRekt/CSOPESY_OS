#pragma once

#include <map>
#include <string>
#include <unordered_map>
#include <cstdint>

class PagingAllocator;  // forward
class SchedAlgo;

class MemoryManager
{
    int max_overall_mem;
    int mem_per_frame;
    int min_mem_per_proc;
    int max_mem_per_proc;

    size_t maximum_size;       // in KiB
    size_t allocated_size{ 0 };  // in KiB

    int page_in{ 0 };
    int page_out{ 0 };

    std::unordered_map<size_t, uint16_t> memory_store;

    bool greaterThanMaxMemory(size_t amount);
    bool lessThanZeroMemory(size_t amount);

public:
    MemoryManager();
    void initialize(std::map<std::string, std::string> config);

    int get_max_overall_mem() const;
    int get_mem_per_frame() const;
    int get_mem_per_proc() const;

    void add_page_in(int frames);
    void add_page_out(int frames);

    size_t get_memory_max_size() const;
    size_t get_memory_allocated_size() const;
    int get_page_in() const;
    int get_page_out() const;

    void add_memory(size_t amount);
    void remove_memory(size_t amount);
    void set_memory(size_t amount);
    int get_mem_util() const;

    void write_memory(size_t addr, uint16_t value);
    uint16_t read_memory(size_t addr);
};

extern MemoryManager memManager;
