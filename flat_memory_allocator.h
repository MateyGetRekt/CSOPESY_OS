#pragma  once
#include "imemory_allocator.h"
#include <unordered_map>
#include <vector>
#include <set>



struct CompareAddress {
    bool operator()(std::tuple<void*, std::string> a, std::tuple<void*, std::string> b) const {
        return std::get<0>(a) > std::get<0>(b);
    }
};

class FlatMemoryAllocator : public IMemoryAllocator
{
    size_t maximum_size;
    std::set<std::tuple<void*, std::string>, CompareAddress> process_directory;
    std::vector<char> memory;
    std::unordered_map<size_t, bool> allocation_map;

    void initialize_memory();
    bool can_allocate_at(size_t index, size_t size) const;
    void allocate_at(size_t index, size_t size);
    void deallocate_at(size_t index, size_t size);
    bool can_allocate(size_t size);
    void add_to_process_tracking(void* memory, std::string process_name);
    void delete_from_process_tracking(void* memory, std::string process_name);

public:
    FlatMemoryAllocator(size_t maximum_size);
    ~FlatMemoryAllocator();
    
    void* allocate(size_t size, size_t processId);
    void  deallocate(void* ptr, size_t size, size_t processId);
    void  visual_memory();
    
};
