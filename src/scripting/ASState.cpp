#include "ASState.h"

asIScriptEngine *ASState::engine = nullptr;

// --- split_headers: auto-generated ---

ASState::ASState() {
    engine = nullptr;
}

asIScriptEngine *ASState::Get() {
    if (engine)
        return engine;

    engine = asCreateScriptEngine();
    assert(engine);

    engine->SetMessageCallback(asFUNCTION(MessageCallback), nullptr, asCALL_CDECL);

    return engine;
}

void ASState::Shutdown() {
    if (engine) {
        engine->ShutDownAndRelease();
        engine = nullptr;
    }
}

void ASState::MessageCallback(const asSMessageInfo *msg, void *) {
    const char *type = "ERR";
    if (msg->type == asMSGTYPE_WARNING)
        type = "WARN";
    else if (msg->type == asMSGTYPE_INFORMATION)
        type = "INFO";

    printf("[AngelScript] %s (%d, %d): %s - %s\n",
           msg->section, msg->row, msg->col, type, msg->message);
}
