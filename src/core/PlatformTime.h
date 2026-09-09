#pragma once
#include <ctime>

inline void PortableLocalTime(const std::time_t &time, std::tm &out) {
#if defined(_WIN32)
    localtime_s(&out, &time);
#else
    localtime_r(&time, &out);
#endif
}