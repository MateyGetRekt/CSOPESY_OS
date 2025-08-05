#include "memory-manager.h"
#include "util.h"
#include "scheduler.h"
#include "sched_algo.h"
#include "pagingAllocator.h"
#include <memory>
#include <stdexcept>

extern Scheduler sched;

// Constructor
MemoryManager::MemoryManager() = default;

// Initializes memory and installs the paging allocator (units in KiB)
void MemoryManager::initialize(std::map<std::string, std::string> config)
{
    max_overall_mem = std::stoi(config["max-overall-mem"]);
    mem_per_frame = std::stoi(config["mem-per-frame"]);
    min_mem_per_proc = std::stoi(config["min-mem-per-proc"]);
    max_mem_per_proc = std::stoi(config["max-mem-per-proc"]);

    maximum_size = static_cast<size_t>(max_overall_mem);
    allocated_size = 0;
    page_in = 0;
    page_out = 0;
    memory_store.clear();

    // Install demand-paging allocator
    SchedAlgo* algo = sched.get_algorithm();
    algo->set_memory_allocator(
        std::unique_ptr<PagingAllocator>(new PagingAllocator(max_overall_mem))
    );
}

int MemoryManager::get_max_overall_mem() const { return max_overall_mem; }
int MemoryManager::get_mem_per_frame()   const { return mem_per_frame; }
int MemoryManager::get_mem_per_proc()    const { return min_mem_per_proc; }

void MemoryManager::add_page_in(int frames) { page_in += frames; }
void MemoryManager::add_page_out(int frames) { page_out += frames; }
size_t MemoryManager::get_memory_max_size() const { return max_overall_mem; }
size_t MemoryManager::get_memory_allocated_size() const { return allocated_size; }
int MemoryManager::get_page_in() const { return page_in; }
int MemoryManager::get_page_out() const { return page_out; }

void MemoryManager::add_memory(size_t amount)
{
    if (!greaterThanMaxMemory(amount)) {
        allocated_size += amount;
    }
}
void MemoryManager::remove_memory(size_t amount)
{
    if (!lessThanZeroMemory(amount)) {
        allocated_size -= amount;
    }
}
void MemoryManager::set_memory(size_t amount)
{
    if (amount <= maximum_size) {
        allocated_size = amount;
    }
}
int MemoryManager::get_mem_util() const
{
    return static_cast<int>((100 * allocated_size) / max_overall_mem);
}

bool MemoryManager::greaterThanMaxMemory(size_t amount)
{
    return (allocated_size + amount) > max_overall_mem;
}
bool MemoryManager::lessThanZeroMemory(size_t amount)
{
    return amount > allocated_size;
}

void MemoryManager::write_memory(size_t addr, uint16_t value)
{
    memory_store[addr] = value;
}
uint16_t MemoryManager::read_memory(size_t addr)
{
    auto it = memory_store.find(addr);
    return (it != memory_store.end() ? it->second : 0);
}
