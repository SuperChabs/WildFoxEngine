#include "ASRegisterAPI.h"
#include "scripting/ASState.h"

void ASRegisterAPI::RegisterAll(ECSWorld *ecs, Input *input, AudioSystem *audioSystem) {
    asIScriptEngine *engine = ASState::Get();

    RegisterStdString(engine);

    RegisterTypes(engine);
    RegisterCommands(ecs);
    RegisterECS(engine, ecs);
    RegisterInput(engine, input);
    RegisterMath(engine);
    RegisterAudio(engine, audioSystem);
}
