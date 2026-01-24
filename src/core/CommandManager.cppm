module;

#include <unordered_map>
#include <functional>
#include <string>
#include <variant>

#include <glm/glm.hpp>

export module WFE.Core.CommandManager;

import WFE.Core.Logger;
import WFE.Core.Camera;

export using CommandArg = std::variant<int, bool, float, std::string, glm::vec3, glm::mat4, Camera*>;
export using CommandArgs = std::vector<CommandArg>;
export using CommandFn = std::function<void(const CommandArgs&)>;

export class CommandManager
{
private:
    static std::unordered_map<std::string, CommandFn> commands;

public:
    static void RegisterCommand(const std::string& name, CommandFn fn)
    {
        if(commands.contains(name))
        {
            Logger::Log(LogLevel::WARNING, LogCategory::CORE, "Command '" + name + "' already exists");
            return;
        }

        commands.emplace(name, std::move(fn));
    }

    static void ExecuteCommand(const std::string& name, const CommandArgs& args)
    {
        auto it = commands.find(name);
        if (it != commands.end())
        {
            // Logger::Log(LogLevel::INFO, "Executing '" + name + "'");
            it->second(args);
        }
        else 
            Logger::Log(LogLevel::ERROR, "Command '" + name + "' not found");
        
    }

    static bool HasCommand(const std::string& name)
    {
        return commands.contains(name);
    }
};

module :private;
std::unordered_map<std::string, CommandFn> CommandManager::commands;