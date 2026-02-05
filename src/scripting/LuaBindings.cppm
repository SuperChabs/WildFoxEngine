export module WFE.Scripting.LuaBindings;

import WFE.ECS.ECSWorld;
import WFE.Scripting.LuaState;
import WFE.Scripting.LuaRegisterAPI;

export void InitLua(ECSWorld* ecs) 
{
    auto& lua = LuaState::Get();

    LuaRegisterAPI::RegisterWorld(ecs, lua);
    LuaRegisterAPI::RegisterEntity(ecs,lua);
    LuaRegisterAPI::RegisterComponent(lua);
}
