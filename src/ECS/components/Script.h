#pragma once

#include <string>

#include <angelscript.h>
#include <scriptbuilder.h>

struct c_Script {
    std::string scriptPath;

    asIScriptModule *module = nullptr;
    asIScriptContext *ctx   = nullptr;

    // life cycle functions
    asIScriptFunction *fnOnStart       = nullptr;
    asIScriptFunction *fnOnUpdate      = nullptr;
    asIScriptFunction *fnOnFixedUpdate = nullptr;
    asIScriptFunction *fnOnStop        = nullptr;
    asIScriptFunction *fnOnDestroy     = nullptr;

    bool loaded = false;
    bool failed = false;
    bool active = true;
};