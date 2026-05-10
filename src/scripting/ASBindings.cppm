export module WFE.Scripting.ASBindings;

import WFE.ECS.ECSWorld;
import WFE.ECS.Systems.AudioSystem;
import WFE.Core.Input;
import WFE.Scripting.ASState;
import WFE.Scripting.ASRegisterAPI;

export void InitAS(ECSWorld* ecs, Input* input, AudioSystem* audioSystem)
{
    ASState::Get();
    ASRegisterAPI::RegisterAll(ecs, input, audioSystem);
}

export void ShutdownAS()
{
    ASState::Shutdown();
}