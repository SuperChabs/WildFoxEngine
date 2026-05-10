module;

#include <angelscript.h>
#include <scriptbuilder.h>
#include <scriptstdstring.h>

export module WFE.Scripting.ASRegisterAPI;

import WFE.Scripting.ASState;
import WFE.ECS.ECSWorld;
import WFE.Core.Input;

import WFE.Scripting.ASRegisterAPI.Commands;
import WFE.Scripting.ASRegisterAPI.ECS;
import WFE.Scripting.ASRegisterAPI.Input;
import WFE.Scripting.ASRegisterAPI.Math;
import WFE.Scripting.ASRegisterAPI.Types;
import WFE.Scripting.ASRegisterAPI.Audio;
import WFE.ECS.Systems.AudioSystem;

export class ASRegisterAPI
{
public:
    static void RegisterAll(ECSWorld* ecs, Input* input, AudioSystem* audioSystem)
    {
        asIScriptEngine* engine = ASState::Get();

        RegisterStdString(engine);

        RegisterTypes(engine);
        RegisterCommands(ecs);
        RegisterECS(engine, ecs);
        RegisterInput(engine, input);
        RegisterMath(engine);
        RegisterAudio(engine, audioSystem);
    }
};