module;

#include <string>
#include <chrono>
#include <cstring>
#include <mutex>
#include <source_location>

export module WildFoxEngine.Core.Logger;

export enum class LogLevel
{
    INFO,
    WARNING,
    ERROR,
    DEBUG,
    INPUT,
    CRITICAL // ,
    // TRACE
};

export enum class LogCategory 
{
    CORE,
    RENDERING,
    UI,
    OTHER
};

export struct LogData
{
    LogLevel lvl;
    LogCategory cat;

    std::string m;
    std::string f;
    int line;
    bool showOrigin;

    std::chrono::system_clock::time_point timestamp;
};

export class ILogSink
{
public:
    virtual ~ILogSink() = default;
    virtual void write(const LogData& data) = 0;
};

export class Logger
{
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


    static void Log(LogLevel level, const std::string& message, bool showOrigin, const std::source_location& loc = std::source_location::current())
    {
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

private:

    static const char* stripProjectRoot(const char* file)
    {
        constexpr const char* ROOT = "WildFoxEngine/"; 
        const char* pos = std::strstr(file, ROOT);
        return pos ? pos : file;
    }

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

};
