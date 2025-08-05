#include "flat_memory_allocator.h"
#include "screen.h"
#include "screenManager.h"
#include "memory-manager.h"
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <mutex>
#include <chrono>

extern MemoryManager memManager;
std::mutex memory_mutex_rr;
extern std::mutex memory_mutex;
int quantum_cycles = 0;  // Counter for memory snapshots

// Helper function to print allocation map (debugging)
static void print_map(std::unordered_map<size_t, bool>& map)
{
    for (const auto& [key, value] : map)
    {
        std::cout
            << "{" << key
            << ": " << value
            << "} ";
    }
    std::cout << "\n";
}

// Initializes memory with default values and marks all as free
void FlatMemoryAllocator::initialize_memory()
{
    std::fill(memory.begin(), memory.end(), '.');  // Fill memory with placeholder
    for (size_t i = 0; i < maximum_size; i++) {
        allocation_map[i] = false;  // Mark all blocks as free
    }
}

// Checks if allocation at given index with given size is possible
bool FlatMemoryAllocator::can_allocate_at(size_t index, size_t size) const
{
    return (index + size <= maximum_size);  // Simple bounds check
}

// Marks memory range as allocated in the allocation map
void FlatMemoryAllocator::allocate_at(size_t index, size_t size)
{
    for (size_t i = 0; i < index + size; i++) {
        allocation_map[i] = true;  // Mark blocks as allocated
    }
    memManager.add_memory(size);    // Update memory manager
}

// Marks memory range as deallocated in the allocation map
void FlatMemoryAllocator::deallocate_at(size_t index, size_t size)
{
    for (size_t i = index; i < index + size; i++) {
        allocation_map[i] = false;  // Mark blocks as free
    }
    memManager.remove_memory(size); // Update memory manager
}

// Constructor - reserves and initializes memory
FlatMemoryAllocator::FlatMemoryAllocator(size_t maximum_size)
    : maximum_size(maximum_size)
{
    memory.reserve(maximum_size);
    memory.resize(maximum_size);
    allocation_map.reserve(maximum_size);
    initialize_memory();
}

// Destructor - clears memory
FlatMemoryAllocator::~FlatMemoryAllocator()
{
    memory.clear();
}

// Main allocation function - finds first fit contiguous block
void* FlatMemoryAllocator::allocate(size_t size, size_t processId = 0) {
    std::unique_lock<std::mutex> memoryLock(memory_mutex);  // Thread safety
    
    // First-fit allocation strategy
    for (size_t i = 0; i < maximum_size - size + 1; ++i) {
        if (!allocation_map[i] && can_allocate_at(i, size)) {
            allocate_at(i, size);
            return &memory[i];  // Return pointer to allocated memory
        }
    }
    return nullptr;  // Allocation failed
}

// Main deallocation function - frees memory block
void FlatMemoryAllocator::deallocate(void* ptr, size_t size, size_t processId = 0)
{
    std::unique_lock<std::mutex> memoryLock(memory_mutex);  // Thread safety
    size_t index = static_cast<char*>(ptr) - &memory[0];    // Calculate index
    
    if (allocation_map[index]) {
        deallocate_at(index, size);
    }
    else {
        screenManager.getCurrentScreen().print("Requested data is not available in the memory.\n");
    }
}

// Checks if contiguous block of given size is available
bool FlatMemoryAllocator::can_allocate(size_t size)
{
    std::unique_lock<std::mutex> memoryLock(memory_mutex);
    int cont_space = 0;
    int index = 0;

    if (size == 0) return true;
    if (size > allocation_map.size()) return false;

    // Scan for contiguous free space
    while (cont_space < size) 
    {
        if (index >= allocation_map.size()) return false;
        if (!allocation_map[index]) cont_space++;
        else cont_space = 0;
        index++;
    }
    return true;
}

// Tracks which process owns which memory block
void FlatMemoryAllocator::add_to_process_tracking(void* memory, std::string process_name)
{
    std::unique_lock<std::mutex> memoryLock(memory_mutex);
    process_directory.insert(std::make_tuple(memory, process_name));
}

// Removes process from memory tracking
void FlatMemoryAllocator::delete_from_process_tracking(void* memory, std::string process_name)
{
    std::unique_lock<std::mutex> memoryLock(memory_mutex);
    process_directory.erase(std::make_tuple(memory, process_name));
}

// Creates memory visualization/snapshot file
void FlatMemoryAllocator::visual_memory()
{
    std::unique_lock<std::mutex> memoryLock(memory_mutex);
    std::filesystem::create_directory("memory_stamp");
    std::ofstream file_stream;
    file_stream.open("memory_stamp/memory_stamp_" + std::to_string(quantum_cycles) + ".txt");
    quantum_cycles++;

    if (file_stream.is_open())
    {
        // Calculate free space (external fragmentation)
        int free_space = 0;
        for (size_t i = 0; i < maximum_size; i++) {
            if (!allocation_map[i]) free_space++;
        }

        // Write timestamp and basic info
        auto current_time = time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now());
        auto const local_time = std::chrono::current_zone()->to_local(current_time);
        file_stream << "Timestamp: " << std::format("({:%m/%d/%Y, %I:%M:%S %p})", local_time) << std::endl;
        file_stream << "Number of processes in the memory: " << process_directory.size() << std::endl;
        file_stream << "Total external fragmentation in KB: " << free_space << std::endl;

        // Write memory bounds
        file_stream << "----end---- = " << maximum_size << std::endl;

        // Write process memory allocations
        for (auto entry : process_directory) {
            int index = (char*)std::get<0>(entry) - &memory[0];
            file_stream << std::endl << index + memManager.get_mem_per_proc() << std::endl;
            file_stream << std::get<1>(entry) << std::endl;
            file_stream << index << std::endl;
        }

        file_stream << "----start---- = 0" << std::endl;
        file_stream.close();
    }
}