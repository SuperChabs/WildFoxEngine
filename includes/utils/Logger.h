#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <iostream>

class Logger
{
public:
    static void Info(const std::string& message)
    {
        std::cout << "[INFO]: " << message << std::endl;
    }

    static void Warning(const std::string& message)
    {
        std::cout << "[WARNING]: " << message << std::endl;
    }

    static void Error(const std::string& message)
    {
        std::cerr << "[ERROR]: " << message << std::endl;
    }

};

#endif // LOGGER_H