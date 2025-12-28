#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <iostream>

enum class LogLevel
{
    INFO,
    WARNING,
    ERROR,
    DEBUG
};

class Logger
{
public:
    static void Log(LogLevel level, const std::string& message)
    {
        switch (level)
        {
            case LogLevel::INFO:
                std::cout << "[INFO]: " << message << std::endl;
                break;
            case LogLevel::WARNING:
                std::cout << "[WARNING]: " << message << std::endl;
                break;
            case LogLevel::ERROR:
                std::cerr << "[ERROR]: " << message << std::endl;
                break;
            case LogLevel::DEBUG:
                std::cout << "[DEBUG]: " << message << std::endl;
                break;
        }
    }

};

#endif // LOGGER_H