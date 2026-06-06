#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <variant>
#include <functional>

#include <glm/glm.hpp>
#include <entt/entt.hpp>

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
    entt::entity
>;

using CommandArgs = std::vector<CommandArg>;
using CommandFn = std::function<void(const CommandArgs &)>;

class CommandManager {
    static std::unordered_map<std::string, CommandFn> commands;

public:
    static void RegisterCommand(const std::string &name, CommandFn fn);

    static void ExecuteCommand(const std::string &name, const CommandArgs &args);

    static bool HasCommand(const std::string &name);
};