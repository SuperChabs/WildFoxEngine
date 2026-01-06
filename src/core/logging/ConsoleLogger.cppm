module;

#include <string>
#include <chrono>
#include <cstdio>

export module XEngine.Core.Logging.ConsoleLogger;

import XEngine.Core.Logger;

export class ConsoleLogger : public ILogSink
{
public:
    void write(const LogData& data) override
    {
        const char* lvl = levelToString(data.lvl);
        const char* cat = categoryToString(data.cat);
        std::string timeStr = formatTime(data.timestamp);

        if (data.showOrigin)
            std::printf("[%s] [%s] [%s] %s (%s:%i)\n", 
                        timeStr.c_str(), 
                        lvl, 
                        cat,
                        data.m.c_str(), 
                        data.f.c_str(), 
                        data.line);
        else
            std::printf("[%s] [%s] [%s] %s\n", 
                        timeStr.c_str(), 
                        lvl,
                        cat, 
                        data.m.c_str());
    }

private:
static const char* levelToString(LogLevel lvl)
{
    switch (lvl)
    {
        case LogLevel::INFO:     
            return "\033[32mINFO\033[0m";        // green
        case LogLevel::WARNING:  
            return "\033[33mWARNING\033[0m";     // yellow
        case LogLevel::ERROR:    
            return "\033[31mERROR\033[0m";       // red
        case LogLevel::DEBUG:    
            return "\033[36mDEBUG\033[0m";       // cian
        case LogLevel::INPUT:    
            return "\033[35mINPUT\033[0m";       // magenta
        case LogLevel::CRITICAL: 
            return "\033[1;31mCRITICAL\033[0m";  // fat yellow
        default:                 
            return "\033[37mUNKNOWN\033[0m";     // gray
    }
}


    static const char* categoryToString(LogCategory cat)
    {
        switch (cat)
        {
            case LogCategory::CORE:      return "CORE";
            case LogCategory::RENDERING: return "RENDERING";
            case LogCategory::OTHER:     return "OTHER";
            default:                     return "UNKNOWN";
        }
    }

    static std::string formatTime(const std::chrono::system_clock::time_point& tp)
    {
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
}; 