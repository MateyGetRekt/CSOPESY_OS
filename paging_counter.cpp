#include "paging_counter.h"
#include <mutex>

void PagingCounter::page_in(std::mutex& mutex)
{
    std::unique_lock<std::mutex> lock(mutex);
    paged_in++;
}

void PagingCounter::page_out(std::mutex& mutex)
{
    std::unique_lock<std::mutex> lock(mutex);
    paged_out++;
}

int PagingCounter::get_paged_in()
{
    return paged_in;
}

int PagingCounter::get_paged_out()
{
    return paged_out;
}
