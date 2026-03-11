module WFE.Scripting.LuaWorldAPI;

#include <sol/sol.hpp>
#include <entt/entity/entity.hpp> 

void LuaWorldAPI::RegisterWorldAPI(WFE::ECSWorld* ecs, sol::state& lua)
{
    lua.set_function("IsValid", [ecs](entt::entity e) {
        return ecs->IsValid(e);
    });

    lua.set_function("DestroyEntity", [ecs](entt::entity e) {
        ecs->DestroyEntity(e);
    });
}