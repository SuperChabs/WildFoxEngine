#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <cstdio>
#include <chrono>
#include <mutex>

enum class LogLevel
{
    INFO,
    WARNING,
    ERROR,
    DEBUG,
    INPUT,
    CRITICAL // ,
    // TRACE
};

enum class LogCategory 
{
    CORE,
    RENDERING,
    OTHER
};

struct LogData
{
    LogLevel lvl;
    LogCategory cat;

    std::string m;
    std::string f;
    int line;
    bool showOrigin;

    std::chrono::system_clock::time_point timestamp;
};

class ILogSink
{
public:
    virtual ~ILogSink() = default;
    virtual void write(const LogData& data) = 0;
};

class Logger
{
private:
    static std::vector<ILogSink*>& sinks() 
    {
        static std::vector<ILogSink*> instance;
        return instance;
    }

    static std::mutex& logMutex() 
    {
        static std::mutex instance;
        return instance;
    }

public:
    static void AddSink(ILogSink* sink)
    {
        std::lock_guard<std::mutex> lock(logMutex());

        sinks().push_back(sink);
    }

    static void RemoveSink(ILogSink* sink)
    {
        std::lock_guard<std::mutex> lock(logMutex());
        auto& s = sinks();
        s.erase(std::remove(s.begin(), s.end(), sink), s.end());
    }

    static void ClearSinks()
    {
        std::lock_guard<std::mutex> lock(logMutex());
        sinks().clear();
    }

    static void Log(LogLevel level, const std::string& message)
    {
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
        
        for (auto* sink : sinks())
            if (sink)
                sink->write(data);
    }

    static void Log(LogLevel level, const std::string& message, bool showOrigin)
    {
        std::lock_guard<std::mutex> lock(logMutex());
        
        LogData data{
            level, 
            LogCategory::OTHER, 
            message, 
            showOrigin ? __FILE__ : "", 
            showOrigin ? __LINE__ : 0, 
            showOrigin, 
            std::chrono::system_clock::now()
        };
        
        for (auto* sink : sinks())
            if (sink)
                sink->write(data);
    }

    static void Log(LogLevel level, LogCategory cat, const std::string& message, bool showOrigin = false)
    {
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
        
        for (auto* sink : sinks())
            if (sink)
                sink->write(data);
    }
};

class ConsoleLogger : public ILogSink
{
public:
    void write(const LogData& data) override
    {
        const char* lvl = levelToString(data.lvl);
        const char* cat = categoryToString(data.cat);
        std::string timeStr = formatTime(data.timestamp);

        if (data.showOrigin)
            std::printf("[%s] [%s] [%s] %s (%s:%s)\n", 
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
            case LogLevel::INFO:     return "INFO";
            case LogLevel::WARNING:  return "WARNING";
            case LogLevel::ERROR:    return "ERROR";
            case LogLevel::DEBUG:    return "DEBUG";
            case LogLevel::INPUT:    return "INPUT";
            case LogLevel::CRITICAL: return "CRITICAL";
            default:                 return "UNKNOWN";
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
#ifdef _WIN32
        localtime_s(&tm_snapshot, &time);
#else
        localtime_r(&time, &tm_snapshot);
#endif
        
        std::ostringstream oss;
        oss << std::put_time(&tm_snapshot, "%H:%M:%S");
        oss << '.' << std::setfill('0') << std::setw(3) << ms.count();
        
        return oss.str();
    }
};

#endif // LOGGER_H