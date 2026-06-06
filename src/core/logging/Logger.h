#pragma once

#include <string>
#include <chrono>
#include <vector>
#include <mutex>
#include <source_location>

enum class LogLevel {
    INFO,
    WARNING,
    ERROR,
    DEBUG,
    INPUT,
    CRITICAL //,
    // TRACE
};

enum class LogCategory {
    CORE,
    RENDERING,
    UI,
    ECS_SYSTEM,
    OTHER
};

struct LogData {
    LogLevel lvl;
    LogCategory cat;

    std::string m;
    std::string f;
    int line;
    bool showOrigin;

    std::chrono::system_clock::time_point timestamp;
};

class ILogSink {
public:
    virtual ~ILogSink() = default;

    virtual void write(const LogData &data) = 0;
};

class Logger {
public:
    static void AddSink(ILogSink *sink);

    static void RemoveSink(ILogSink *sink);

    static void ClearSinks();

    static void Log(LogLevel level, const std::string &message);

    static void Log(LogLevel level, const std::string &message, bool showOrigin,
                    const std::source_location &loc = std::source_location::current());

    static void Log(LogLevel level, LogCategory cat, const std::string &message, bool showOrigin = false);

private:
    static const char *stripProjectRoot(const char *file);

    static std::vector<ILogSink *> &sinks();

    static std::mutex &logMutex();
};