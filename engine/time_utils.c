#include "time_utils.h"

#ifdef _WIN32
#include <windows.h>
long long GetTimeMs(void) {
    return GetTickCount64();
}
#else
#include <sys/time.h>
#include <stddef.h>
long long GetTimeMs(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}
#endif