#include "FileLogger.h"

#include <ctime>
#include <iomanip>
#include <sstream>
#include <filesystem>

namespace fs = std::filesystem;

FileLogger::FileLogger()
    : folderName("../log") {
    if (!fs::exists("../log"))
        fs::create_directory(folderName);
}

FileLogger::~FileLogger() {
    isShuttingDown.store(true);

    std::lock_guard<std::mutex> lock(fileMutex);
    if (file.is_open()) {
        file.flush();
        file.close();
    }
}

void FileLogger::write(const LogData &data) {
    if (isShuttingDown.load()) return;

    std::lock_guard<std::mutex> lock(fileMutex);

    if (!file.is_open())
        OpenLogFile(data);

    const char *lvl = LevelToString(data.lvl);
    const char *cat = CategoryToString(data.cat);

    if (file.is_open()) {
        if (data.showOrigin)
            file << std::format("[{}] [{}] [{}] {} ({}:{})",
                                timeStr,
                                lvl,
                                cat,
                                data.m,
                                data.f,
                                data.line) << std::endl;
        else
            file << std::format("[{}] [{}] [{}] {}",
                                timeStr,
                                lvl,
                                cat,
                                data.m) << std::endl;

        file.flush();
    }
}

void FileLogger::OpenLogFile(const LogData &data) {
    timeStr = FormatTime(data.timestamp);
    std::string dateStr = FormatDate(data.timestamp);

    logPath = folderName + "/" + std::format("log-{}-{}.txt", dateStr, timeStr);
    file.open(logPath, std::ios::app);
}

const char *FileLogger::LevelToString(LogLevel lvl) {
    switch (lvl) {
        case LogLevel::INFO: return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERROR: return "ERROR";
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INPUT: return "INPUT";
        case LogLevel::CRITICAL: return "CRITICAL";
        default: return "UNKNOWN";
    }
}


const char *FileLogger::CategoryToString(LogCategory cat) {
    switch (cat) {
        case LogCategory::CORE: return "CORE";
        case LogCategory::RENDERING: return "RENDERING";
        case LogCategory::UI: return "UI";
        case LogCategory::OTHER: return "OTHER";
        default: return "UNKNOWN";
    }
}

std::string FileLogger::FormatTime(const std::chrono::system_clock::time_point &tp) {
    auto time = std::chrono::system_clock::to_time_t(tp);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                  tp.time_since_epoch()
              ) % 1000;

    std::tm tm_snapshot;

    localtime_r(&time, &tm_snapshot); // only for unix-like systems

    std::ostringstream oss;
    oss << std::put_time(&tm_snapshot, "%H:%M:%S");
    oss << '.' << std::setfill('0') << std::setw(3) << ms.count();

    return oss.str();
}

std::string FileLogger::FormatDate(const std::chrono::system_clock::time_point &tp) {
    auto time = std::chrono::system_clock::to_time_t(tp);
    std::tm tm_snapshot;

    localtime_r(&time, &tm_snapshot);

    std::ostringstream oss;
    oss << std::put_time(&tm_snapshot, "%Y-%m-%d");
    return oss.str();
}