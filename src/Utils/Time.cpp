#include "Time.h"

#include <chrono>
#include "Logger.h"

uint64_t Time::timestamp = 0;

uint64_t Time::getEpochTimeMillisUTC() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
}

void Time::setInitialTimestamp() {
    Logger::info("setting timestamp");
    timestamp = getEpochTimeMillisUTC();
}

uint64_t Time::getSetedTimestamp() {
    return timestamp;
}