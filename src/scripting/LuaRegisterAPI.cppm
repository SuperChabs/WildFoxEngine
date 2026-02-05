module;

#include <sol/sol.hpp>
#include <entt/entity/fwd.hpp>
#include <lua.hpp>
#include <glm/glm.hpp>

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

        lua.set_function("GetTransform", [ecs](entt::entity e) -> TransformComponent*
        {
            if (!ecs->HasComponent<TransformComponent>(e))
                return nullptr;

            return &ecs->GetComponent<TransformComponent>(e);
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
        lua.new_usertype<glm::vec3>("vec3",
            sol::constructors<
                glm::vec3(),
                glm::vec3(float, float, float)
            >(),
            "x", &glm::vec3::x,
            "y", &glm::vec3::y,
            "z", &glm::vec3::z
        );

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