#pragma once

#include <angelscript.h>
#include <scriptbuilder.h>

#include <cassert>

class ASState {
    static asIScriptEngine *engine;

public:
    ASState() {
        engine = nullptr;
    }

    static asIScriptEngine *Get() {
        if (engine)
            return engine;

        engine = asCreateScriptEngine();
        assert(engine);

        engine->SetMessageCallback(asFUNCTION(MessageCallback), nullptr, asCALL_CDECL);

        return engine;
    }

    static void Shutdown() {
        if (engine) {
            engine->ShutDownAndRelease();
            engine = nullptr;
        }
    }

private:
    static void MessageCallback(const asSMessageInfo *msg, void *) {
        const char *type = "ERR";
        if (msg->type == asMSGTYPE_WARNING)
            type = "WARN";
        else if (msg->type == asMSGTYPE_INFORMATION)
            type = "INFO";

        printf("[AngelScript] %s (%d, %d): %s - %s\n",
               msg->section, msg->row, msg->col, type, msg->message);
    }
};