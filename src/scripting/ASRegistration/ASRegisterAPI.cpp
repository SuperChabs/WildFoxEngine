#include "ASRegisterAPI.h"

#include "scripting/ASState.h"
#include "scripting/ASRegistration/ASRegisterCommands.h"
#include "scripting/ASRegistration/ASRegisterECS.h"
#include "scripting/ASRegistration/ASRegisterInput.h"
#include "scripting/ASRegistration/ASRegisterMath.h"
#include "scripting/ASRegistration/ASRegisterTypes.h"
#include "scripting/ASRegistration/ASRegisterAudio.h"
#include "scripting/ASRegistration/ASLogging.h"

void ASRegisterAPI::RegisterAll(ECSWorld *ecs, Input *input, AudioSystem *audioSystem) {
    asIScriptEngine *engine = ASState::Get();

    RegisterStdString(engine);

    RegisterTypes(engine);
    RegisterCommands(ecs);
    RegisterECS(engine, ecs);
    RegisterInput(engine, input);
    RegisterMath(engine);
    RegisterAudio(engine, audioSystem);
    RegisterLogging(engine);
}
