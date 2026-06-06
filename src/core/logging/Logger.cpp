#include "Logger.h"

#include <cstring>

void Logger::Logger::AddSink(ILogSink *sink) {
    std::lock_guard<std::mutex> lock(logMutex());

    sinks().push_back(sink);
}

void Logger::RemoveSink(ILogSink *sink) {
    std::lock_guard<std::mutex> lock(logMutex());
    auto &s = sinks();
    s.erase(std::remove(s.begin(), s.end(), sink), s.end());
}

void Logger::ClearSinks() {
    std::lock_guard<std::mutex> lock(logMutex());
    sinks().clear();
}

void Logger::Log(LogLevel level, const std::string &message) {
    std::lock_guard<std::mutex> lock(logMutex());

    LogData data{
        level,
        LogCategory::OTHER,
        message,
        "",
        0,
        false,
        std::chrono::system_clock::now()
    };

    for (auto *sink: sinks())
        if (sink)
            sink->write(data);
}


void Logger::Log(LogLevel level, const std::string &message, bool showOrigin, const std::source_location &loc) {
    std::lock_guard<std::mutex> lock(logMutex());

    LogData data{
        level,
        LogCategory::OTHER,
        message,
        showOrigin ? stripProjectRoot(loc.file_name()) : "",
        showOrigin ? int(loc.line()) : 0,
        showOrigin,
        std::chrono::system_clock::now()
    };

    for (auto *sink: sinks())
        if (sink)
            sink->write(data);
}

void Logger::Log(LogLevel level, LogCategory cat, const std::string &message, bool showOrigin) {
    std::lock_guard<std::mutex> lock(logMutex());

    LogData data{
        level,
        cat,
        message,
        showOrigin ? __FILE__ : "",
        showOrigin ? __LINE__ : 0,
        showOrigin,
        std::chrono::system_clock::now()
    };

    for (auto *sink: sinks())
        if (sink)
            sink->write(data);
}

const char *Logger::stripProjectRoot(const char *file) {
    constexpr const char *ROOT = "WFE/";
    const char *pos = std::strstr(file, ROOT);
    return pos ? pos : file;
}

std::vector<ILogSink *> &Logger::sinks() {
    static std::vector<ILogSink *> instance;
    return instance;
}

std::mutex &Logger::logMutex() {
    static std::mutex instance;
    return instance;
}