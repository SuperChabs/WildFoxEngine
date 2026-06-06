#pragma once

#include <string>
#include <fstream>
#include <mutex>
#include <atomic>
#include <chrono>

#include "Logger.h"

class FileLogger : public ILogSink {
    std::string folderName;
    std::string timeStr;
    std::string logPath;
    std::ofstream file;

    std::atomic<bool> isShuttingDown{false};
    std::mutex fileMutex;

public:
    FileLogger();

    ~FileLogger();

    void write(const LogData &data) override;

private:
    void OpenLogFile(const LogData &data);

    const char *LevelToString(LogLevel lvl);

    const char *CategoryToString(LogCategory cat);

    std::string FormatTime(const std::chrono::system_clock::time_point &tp);

    std::string FormatDate(const std::chrono::system_clock::time_point &tp);
};