#pragma once

#include <angelscript.h>
#include <scriptbuilder.h>

class ASState {
    static asIScriptEngine *engine;

public:
    ASState();

    static asIScriptEngine *Get();

    static void Shutdown();

private:
    static void MessageCallback(const asSMessageInfo *msg, void *);
};