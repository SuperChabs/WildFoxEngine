#pragma once

#include <string>
#include <chrono>

#include "Logger.h"

class ConsoleLogger : public ILogSink {
public:
    void write(const LogData &data) override;

private:
    const char *levelToString(LogLevel lvl);

    const char *categoryToString(LogCategory cat);

    std::string formatTime(const std::chrono::system_clock::time_point &tp);
};