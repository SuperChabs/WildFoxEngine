module;

#include <string>
#include <angelscript.h>

export module WFE.ECS.Components.Script;

export struct ScriptComponent
{
    std::string scriptPath;

    asIScriptModule*  module  = nullptr;
    asIScriptContext* ctx     = nullptr;

    asIScriptFunction* fnOnStart  = nullptr;
    asIScriptFunction* fnOnUpdate = nullptr;
    asIScriptFunction* fnOnStop   = nullptr;
    
    bool loaded = false;
    bool failed = false;
    bool active = true;
};