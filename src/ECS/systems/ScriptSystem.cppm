module;

#include <entt/entt.hpp>
#include <sol/sol.hpp>
#include <sol/load_result.hpp>
#include <sol/types.hpp>

export module WFE.ECS.Systems.ScriptSystem;

import WFE.ECS.ECSWorld;
import WFE.ECS.Components;
import WFE.Scripting.LuaState;

export class ScriptSystem
{
    void Update (ECSWorld& ecs, float deltaTime)
    {
        auto& lua = LuaState::Get();

        ecs.Each<ScriptComponent>([&](entt::entity e, ScriptComponent& script) 
        {
            if (!script.initialized)
            {
                script.env = sol::environment(lua, sol::create, lua.globals());

                script.env["entity"] = e;
                script.env["deltaTime"] = deltaTime;

                sol::load_result chunk = lua.load_file(script.scriptPath);
                chunk(script.env);

                if (script.env["onCreate"].valid())
                    script.env["onCreate"]();

                script.initialized = true;
            }

            if (script.env["onUpdate"].valid())
            {
                script.env["deltaTime"] = deltaTime;
                script.env["onUpdate"]();
            }
        });
    }
};