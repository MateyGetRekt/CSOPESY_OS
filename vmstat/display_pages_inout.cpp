#include "display_pages_inout.h"
#include <iostream>
#include <format>
#include "../memory-manager.h"

void display_pages_inout()
{
    int pages_in = memManager.get_page_in();
    int pages_out = memManager.get_page_out();
    std::cout << std::format("{:13} pages swapped in", pages_in) << "\n";
    std::cout << std::format("{:13} pages swapped out", pages_out) << "\n";
}
