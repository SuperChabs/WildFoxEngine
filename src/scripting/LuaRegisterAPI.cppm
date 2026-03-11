module;

#include <sol/sol.hpp>
#include <entt/entity/fwd.hpp>
#include <lua.hpp>
#include <glm/glm.hpp>

export module WFE.Scripting.LuaRegisterAPI;

import WFE.ECS.ECSWorld;
import WFE.ECS.Components;
import WFE.Core.Input;

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

    static void RegisterInput(Input* input, sol::state& lua)
    {
        // Реєстрація enum Key
        lua.new_enum("Key", 
            "W", Key::KEY_W,
            "A", Key::KEY_A,
            "S", Key::KEY_S,
            "D", Key::KEY_D,
            "Q", Key::KEY_Q,
            "E", Key::KEY_E,
            "UP", Key::KEY_UP,
            "DOWN", Key::KEY_DOWN,
            "LEFT", Key::KEY_LEFT,
            "RIGHT", Key::KEY_RIGHT,
            "SPACE", Key::KEY_SPACE,
            "LEFT_SHIFT", Key::KEY_LEFT_SHIFT,
            "LEFT_CONTROL", Key::KEY_LEFT_CONTROL,
            "ESCAPE", Key::KEY_ESCAPE,
            "ENTER", Key::KEY_ENTER,
            "TAB", Key::KEY_TAB
        );

        // Функції для перевірки клавіш
        lua.set_function("IsKeyPressed", [input](Key key)
        {
            return input->IsKeyPressed(key);
        });

        lua.set_function("IsKeyJustPressed", [input](Key key)
        {
            return input->IsKeyJustPressed(key);
        });

        lua.set_function("IsKeyReleased", [input](Key key)
        {
            return input->IsKeyReleased(key);
        });

        // Функції для миші
        lua.set_function("GetMouseDelta", [input]()
        {
            glm::vec2 delta = input->GetMouseDelta();
            return std::make_pair(delta.x, delta.y);
        });

        lua.set_function("IsMouseButtonPressed", [input](int button)
        {
            return input->IsMouseButtonPressed(button);
        });

        lua.set_function("ResetMouseDelta", [input]()
        {
            input->ResetMouseDelta();
        });

        lua.set_function("GetScrollOffset", [input]()
        {
            return input->GetScrollOffset();
        });
    }
};