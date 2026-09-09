#pragma once

#include <string>
#include <chrono>

#include "Logger.h"

class ConsoleLogger : public ILogSink {
public:
    void write(const LogData &data) override;

private:
    static static const char *levelToString(LogLevel lvl);

    static const char *categoryToString(LogCategory cat);

    static static std::string formatTime(const std::chrono::system_clock::time_point &tp);
};