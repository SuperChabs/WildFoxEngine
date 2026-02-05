module;

#include <entt/entt.hpp>
#include <sol/sol.hpp>
#include <filesystem>

export module WFE.ECS.Systems.ScriptSystem;

import WFE.ECS.ECSWorld;
import WFE.ECS.Components;
import WFE.Scripting.LuaState;
import WFE.Core.Logger;

export class ScriptSystem
{
public:
    void Update(ECSWorld& ecs, float deltaTime)
    {
        auto& lua = LuaState::Get();

        ecs.Each<ScriptComponent>([&](entt::entity e, ScriptComponent& script)
        {
            if (script.failed)
                return;

            if (!script.loaded)
            {
                LoadScript(e, script, lua);
                return;
            }

            // Update
            auto onUpdate = script.env["onUpdate"];
            if (!onUpdate.valid())
                return;

            script.env["deltaTime"] = deltaTime;

            auto result = onUpdate();
            if (!result.valid())
            {
                sol::error err = result;
                Logger::Log(LogLevel::ERROR,
                    "Lua onUpdate error: " + std::string(err.what()));
            }
        });
    }

private:
    void LoadScript(entt::entity e, ScriptComponent& script, sol::state& lua)
    {
        if (!std::filesystem::exists(script.scriptPath))
        {
            Logger::Log(LogLevel::ERROR,
                "Script file not found: " + script.scriptPath);
            script.failed = true;
            return;
        }

        script.env = sol::environment(lua, sol::create, lua.globals());
        script.env["entity"] = e;

        auto load = lua.safe_script_file(script.scriptPath, script.env);
        if (!load.valid())
        {
            sol::error err = load;
            Logger::Log(LogLevel::ERROR,
                "Lua load error: " + std::string(err.what()));
            script.failed = true;
            return;
        }

        // onCreate
        auto onCreate = script.env["onCreate"];
        if (onCreate.valid())
        {
            auto r = onCreate();
            if (!r.valid())
            {
                sol::error err = r;
                Logger::Log(LogLevel::ERROR,
                    "Lua onCreate error: " + std::string(err.what()));
                script.failed = true;
                return;
            }
        }

        script.loaded = true;

        Logger::Log(LogLevel::INFO,
            "Script loaded: " + script.scriptPath);
    }
};
