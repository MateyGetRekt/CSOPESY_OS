#include "util.h"

size_t convert_kb_to_b(size_t kbytes)
{
    return kbytes * 1000;
}

size_t convert_b_to_kb(size_t bytes)
{
    return bytes / 1000;
}
