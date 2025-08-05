#pragma once

#include <string>
class IMemoryAllocator
{
public:
    virtual void* allocate(size_t size, size_t processId = 0) = 0;
    virtual void deallocate(void* ptr, size_t size, size_t processId = 0) = 0;
    virtual bool can_allocate(size_t size) = 0;
    virtual void visual_memory() = 0;
    virtual void add_to_process_tracking(void* memory, std::string process_name) = 0;
    virtual void delete_from_process_tracking(void* memory, std::string process_name) = 0;
};
