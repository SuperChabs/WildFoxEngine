#include "CommandManager.h"

#include "core/logging/Logger.h"

std::unordered_map<std::string, CommandFn> CommandManager::commands;

void CommandManager::RegisterCommand(const std::string &name, CommandFn fn) {
    if (commands.contains(name)) {
        Logger::Log(LogLevel::WARNING, LogCategory::CORE, "Command '" + name + "' already exists");
        return;
    }

    commands.emplace(name, std::move(fn));
}

void CommandManager::ExecuteCommand(const std::string &name, const CommandArgs &args) {
    auto it = commands.find(name);
    if (it != commands.end()) {
        // Logger::Log(LogLevel::INFO, "Executing '" + name + "'");
        it->second(args);
    } else
        Logger::Log(LogLevel::ERROR, "Command '" + name + "' not found");
}

bool CommandManager::HasCommand(const std::string &name) {
    return commands.contains(name);
}