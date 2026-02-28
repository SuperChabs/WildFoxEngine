export module WFE.Scripting.ASBindings;

import WFE.ECS.ECSWorld;
import WFE.Core.Input;
import WFE.Scripting.ASState;
import WFE.Scripting.ASRegisterAPI;

export void InitAS(ECSWorld* ecs, Input* input)
{
    ASState::Get();
    ASRegisterAPI::RegisterAll(ecs, input);
}

export void ShutdownAS()
{
    ASState::Shutdown();
}