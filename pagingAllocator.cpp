#include "pagingAllocator.h"
#include "screen.h"
#include "screenManager.h"
#include "rr.h"

#include <algorithm>
#include <filesystem>
#include <iterator>
#include <ranges>
#include <iostream>
#include <fstream>
#include <mutex>
#include <chrono>
#include <string>

#include "memory-manager.h"

extern MemoryManager memManager;

// Constructor: Initializes the paging allocator with maximum memory size
// Creates a list of free frames (0 to numFrames-1)
PagingAllocator::PagingAllocator(size_t maxMemorySize)
    : maximum_size(maxMemorySize), numFrames(maxMemorySize) {
    // Initialize the free frame list
    for (size_t i = 0; i < numFrames; i++) {
        freeFrameList.push_back(i);
    }
}

// Allocates memory for a process by converting memory needed to frames
// Returns nullptr if not enough frames available, tries reclaiming from backing-store
void* PagingAllocator::allocate(size_t memoryNeeded, size_t processId) {
    // Calculate number of frames needed based on frame size
    size_t numFramesNeeded = (memoryNeeded + memManager.get_mem_per_frame() - 1) / memManager.get_mem_per_frame();

    if (numFramesNeeded > freeFrameList.size()) {
        // Attempt to reclaim from backing-store
        std::ifstream bs("csopesy-backing-store.txt");
        std::string line;
        while (std::getline(bs, line)) {
            auto pos = line.find_last_of(' ');
            size_t fidx = std::stoul(line.substr(pos + 1));
            freeFrameList.push_back(fidx);
        }
        bs.close();

        // Retry after reclaim
        if (numFramesNeeded > freeFrameList.size()) {
            std::cerr << "Memory allocation failed even after reclaim.\n";
            return nullptr;
        }
        std::cerr << "Reclaimed pages from backing-store.\n";
    }

    // Allocate frames and update memory manager
    size_t frameIndex = allocateFrames(numFramesNeeded, processId);
    memManager.add_memory(numFramesNeeded * memManager.get_mem_per_frame());
    return reinterpret_cast<void*>(frameIndex);
}

// Deallocates all frames associated with a process ID
// Updates memory manager with freed memory and logs evictions
void PagingAllocator::deallocate(void* ptr, size_t size, size_t processId) {
    // Find and deallocate all frames mapped to this process
    auto it = std::find_if(frameMap.begin(), frameMap.end(),
        [processId](const auto& entry) { return entry.second == processId; });

    while (it != frameMap.end()) {
        size_t frameIndex = it->first;
        deallocateFrames(1, frameIndex);
        it = std::find_if(frameMap.begin(), frameMap.end(),
            [processId](const auto& entry) { return entry.second == processId; });
    }
    memManager.remove_memory(size);
}

// Internal frame allocation function
// Maps frames to process ID and updates memory manager
size_t PagingAllocator::allocateFrames(size_t numFrames, size_t processId) {
    // Get frame from free list
    size_t frameIndex = freeFrameList.back();
    freeFrameList.pop_back();

    // Map frames to process and update page count
    for (size_t i = 0; i < numFrames; i++) {
        frameMap[frameIndex + i] = processId;
        memManager.add_page_in(1);
    }

    return frameIndex;
}

// Internal frame deallocation function
// Removes frame mapping, returns frames to free list, updates page count, and logs to backing-store
void PagingAllocator::deallocateFrames(size_t numFrames, size_t frameIndex) {
    // Remove frame mapping
    for (size_t i = 0; i < numFrames; i++) {
        frameMap.erase(frameIndex + i);
    }

    // Return frames to free list, update page count, and log eviction
    for (size_t i = 0; i < numFrames; i++) {
        freeFrameList.push_back(frameIndex + i);
        memManager.add_page_out(1);
        // Append evicted page info to backing-store
        std::ofstream bs("csopesy-backing-store.txt", std::ios::app);
        bs << "Evicted frame " << (frameIndex + i) << "\n";
    }
}

// Checks if requested memory can be allocated
// Calculates needed frames and compares with available free frames
bool PagingAllocator::can_allocate(size_t size) {
    const size_t FRAME_SIZE = memManager.get_mem_per_frame();
    // Calculate frames needed with ceiling division
    size_t numFramesNeeded = (size + FRAME_SIZE - 1) / FRAME_SIZE;
    return numFramesNeeded <= freeFrameList.size();
}

// Placeholder functions for process tracking 
void PagingAllocator::add_to_process_tracking(void* memory, std::string process_name) {}
void PagingAllocator::delete_from_process_tracking(void* memory, std::string process_name) {}

// Placeholder for memory visualization function
void PagingAllocator::visual_memory() {}
