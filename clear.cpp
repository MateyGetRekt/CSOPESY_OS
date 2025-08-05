#include "commands.h"
#include <cstdlib>

void clear()
{
#if defined(_WIN32)
    std::system("cls");
#elif defined (__linux__)
    std::system("clear");
#endif
}
