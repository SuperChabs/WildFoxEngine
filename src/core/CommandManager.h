#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <variant>
#include <functional>

#include <glm/glm.hpp>
#include <entt/entt.hpp>
#include <glad/glad.h>

#include "physics/collision/narrowphase/support/constraint/Constraint.h"

using CommandArg =
std::variant<
    int,
    bool,
    float,
    std::string,
    glm::vec3,
    glm::mat4,
    std::vector<glm::mat4>,
    std::vector<int>,
    entt::entity,
    GLuint,
    std::unordered_map<entt::entity, int>,
    std::vector<Constraint*>
>;

using CommandArgs = std::vector<CommandArg>;
using CommandFn = std::function<void(const CommandArgs &)>;

void LogCommandArgError(const std::string &command, size_t index);

/// @brief Typed access to a command argument
/// @return pointer to the argument, or nullptr (with an error logged) if it is missing or has another type
template<typename T>
const T *GetArg(const CommandArgs &args, const size_t index, const std::string &command) {
    const T *value = index < args.size() ? std::get_if<T>(&args[index]) : nullptr;
    if (!value)
        LogCommandArgError(command, index);
    return value;
}

class CommandManager {
    static std::unordered_map<std::string, CommandFn> commands;

public:
    static void RegisterCommand(const std::string &name, CommandFn fn);

    static void ExecuteCommand(const std::string &name, const CommandArgs &args);

    static bool HasCommand(const std::string &name);

    static const std::unordered_map<std::string, CommandFn>& GetCommands() { return commands; }
};