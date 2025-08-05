#pragma once

#include <mutex>
class PagingCounter
{
    int paged_in = 0;
    int paged_out = 0;
public:
    void page_in(std::mutex& mutex);
    void page_out(std::mutex& mutex);

    int get_paged_in();
    int get_paged_out();
};

