#include "ConsoleLogger.h"

#include <ctime>
#include <iomanip>
#include <sstream>
#include <print>

#include "core/PlatformTime.h"

void ConsoleLogger::write(const LogData &data) {
    const char *lvl = levelToString(data.lvl);
    // const char *cat = categoryToString(data.cat);
    const std::string timeStr = formatTime(data.timestamp);

    if (data.showOrigin)
        std::println("[{}] [{}] {} ({}:{})",
                     timeStr,
                     lvl,
                     //cat,
                     data.m,
                     data.f,
                     data.line);
    else
        std::println("[{}] [{}] {}",
                     timeStr,
                     lvl,
                     //cat,
                     data.m);
}

const char *ConsoleLogger::levelToString(const LogLevel lvl) {
    switch (lvl) {
        case LogLevel::INFO:
            return "\033[32mINFO\033[0m"; // green
        case LogLevel::WARNING:
            return "\033[33mWARNING\033[0m"; // yellow
        case LogLevel::ERROR:
            return "\033[31mERROR\033[0m"; // red
        case LogLevel::DEBUG:
            return "\033[36mDEBUG\033[0m"; // cian
        case LogLevel::INPUT:
            return "\033[35mINPUT\033[0m"; // magenta
        case LogLevel::CRITICAL:
            return "\033[1;31mCRITICAL\033[0m"; // fat yellow
        case LogLevel::SCRIPT:
            return "\033[1;33mSCRIPT\033[0m";
        default:
            return "\033[37mUNKNOWN\033[0m"; // gray
    }
}


const char *ConsoleLogger::categoryToString(const LogCategory cat) {
    switch (cat) {
        case LogCategory::CORE: return "CORE";
        case LogCategory::RENDERING: return "RENDERING";
        case LogCategory::UI: return "UI";
        case LogCategory::OTHER: return "OTHER";
        default: return "UNKNOWN";
    }
}

std::string ConsoleLogger::formatTime(const std::chrono::system_clock::time_point &tp) {
    const auto time = std::chrono::system_clock::to_time_t(tp);
    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                  tp.time_since_epoch()
              ) % 1000;

    std::tm tm_snapshot;

    PortableLocalTime(time, tm_snapshot);

    std::ostringstream oss;
    oss << std::put_time(&tm_snapshot, "%H:%M:%S");
    oss << '.' << std::setfill('0') << std::setw(3) << ms.count();

    return oss.str();
}