module;

#include <sol/sol.hpp>
#include <entt/entity/fwd.hpp>
#include <lua.hpp>

export module WFE.Scripting.LuaRegisterAPI;

import WFE.ECS.ECSWorld;
import WFE.ECS.Components;

export class LuaRegisterAPI
{
public:
    static void RegisterWorld(ECSWorld* ecs, sol::state& lua)
    {
        lua.set_function("IsValid", [ecs](entt::entity e) 
        {
            return ecs->IsValid(e);
        });

        lua.set_function("DestroyEntity", [ecs](entt::entity e) 
        {
            ecs->DestroyEntity(e);
        });
    }

    static void RegisterEntity(ECSWorld* ecs, sol::state& lua)
    {
        lua.new_usertype<entt::entity>("Entity");

        lua.set_function("AddComponent", [ecs](entt::entity e, const std::string& name)
        {
            if (name == "Transform")
                ecs->AddComponent<TransformComponent>(e);
        });
    }

    static void RegisterComponent(sol::state& lua)
    {
        lua.new_usertype<TransformComponent>("Transform",
            "position", &TransformComponent::position,
            "rotation", &TransformComponent::rotation,
            "scale",    &TransformComponent::scale
        );

        lua.new_usertype<LightComponent>("Lighting", 
            "position", &LightComponent::position,
            "direction", &LightComponent::direction,
            "ambient", &LightComponent::ambient,
            "diffuse", &LightComponent::diffuse,
            "specular", &LightComponent::specular
        );
    }
};