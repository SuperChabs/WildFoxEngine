module;

#include <angelscript.h>
#include <assert.h>
#include <cstdio>

export module WFE.Scripting.ASState;

export class ASState
{
    static asIScriptEngine* engine;

public:
    static asIScriptEngine* Get()
    {
        if (engine)
            return engine;

        engine = asCreateScriptEngine();
        assert(engine);

        engine->SetMessageCallback(asFUNCTION(MessageCallback), nullptr, asCALL_CDECL);

        return engine;
    }

    static void Shutdown()
    {
        if (engine)
        {
            engine->ShutDownAndRelease();
            engine = nullptr;
        }
    }

private:
    static void MessageCallback(const asSMessageInfo* msg, void*)
    {
        const char* type = "ERR";
        if (msg->type == asMSGTYPE_WARNING)
            type = "WARN";
        else if (msg->type == asMSGTYPE_INFORMATION)
            type = "INFO";

        printf("[AngelScript] %s (%d, %d): %s - %s\n",
            msg->section, msg->row, msg->col, type, msg->message);
    }
};

module :private;
asIScriptEngine* ASState::engine = nullptr;