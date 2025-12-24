#include "ClockTime.h"

#include <chrono>
#include <ctime>

ClockTime getLocalClockTime() {
    using namespace std::chrono;

    auto now = system_clock::now();
    std::time_t now_c = system_clock::to_time_t(now);
    std::tm local{};

#if defined(_WIN32)
    localtime_s(&local, &now_c);
#else
    local = *std::localtime(&now_c);
#endif

    int hour12 = local.tm_hour % 12;
    if (hour12 == 0) hour12 = 12;

    return {
        hour12,
        local.tm_min,
        local.tm_sec
    };
}
