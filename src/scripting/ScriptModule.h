#pragma once

#include <memory>

#include "core/ModuleManager.h"
#include "MainScript.h"
#include "core/IModule.h"
#include "ECS/systems/ScriptSystem.h"
#include "LevelScript.h"

class ScriptModule : public IModule {
    std::unique_ptr<MainScript> m_mainScript;
    std::unique_ptr<ScriptSystem> m_scriptSystem;
    std::unique_ptr<LevelScript> m_levelScript;

    ModuleManager *m_moduleManager;

public:
    ScriptModule(ModuleManager *mm, const std::string &scriptPath = "../assets/scripts/main.as");

    bool Initialize() override;

    void Update(float deltaTime) override;

    void Shutdown() override;

    const char *GetName() const override { return "ScriptModule"; }
    int GetPriority() const override { return 80; }
    bool IsRequired() const override { return true; }

    MainScript *GetMainScript() const { return m_mainScript.get(); }
    LevelScript *GetLevelScript() const { return m_levelScript.get(); }
    ScriptSystem *GetScriptSystem() const { return m_scriptSystem.get(); }
};