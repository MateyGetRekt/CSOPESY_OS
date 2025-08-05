#pragma once
#include "imemory_allocator.h"
#include "memory-manager.h"

#include <unordered_map>
#include <vector>
#include <set>

class PagingAllocator : public IMemoryAllocator 
{
    size_t maximum_size;
    size_t numFrames;
    std::vector<size_t> freeFrameList;
    std::unordered_map<size_t, size_t> frameMap;

    size_t allocateFrames(size_t numFrames, size_t processId);
    void deallocateFrames(size_t numFrames, size_t frameIndex);
    bool can_allocate(size_t size);
    void add_to_process_tracking(void* memory, std::string process_name);
    void delete_from_process_tracking(void* memory, std::string process_name);

public:
    PagingAllocator(size_t maxMemorySize);

    void visual_memory();
    void* allocate(size_t size, size_t processId);
    void deallocate(void* ptr, size_t size, size_t processId);
};