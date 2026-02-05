module;

#include <string>
#include <sol/sol.hpp>

export module WFE.ECS.Components.Script;

export struct ScriptComponent
{
    std::string scriptPath;
    sol::environment env;
    bool initialized = false;
};