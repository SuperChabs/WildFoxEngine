#pragma once

#include <string>

#include <angelscript.h>
#include <entt/entt.hpp>

class MainScript {
    std::string m_scriptPath;

    asIScriptModule  *m_module = nullptr;
    asIScriptContext *m_ctx    = nullptr;

    asIScriptFunction *m_fnOnStart  = nullptr;
    asIScriptFunction *m_fnOnUpdate = nullptr;
    asIScriptFunction *m_fnOnStop   = nullptr;

    bool m_loaded = false;
    bool m_failed = false;

public:
    explicit MainScript(const std::string &scriptPath);

    void Update(float deltaTime);

    void Start();
    void Stop();

    void SetScriptPath(const std::string &scriptPath) { m_scriptPath = scriptPath; }
    std::string GetScriptPath() { return m_scriptPath; }

private:
    void LoadScript();

    void CallUpdate(float deltaTime);
    void CallFunction(asIScriptFunction *fn);
};
