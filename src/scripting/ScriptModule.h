#pragma once

#include <memory>

#include "core/ModuleManager.h"
#include "MainScript.h"
#include "core/IModule.h"
#include "ECS/systems/ScriptSystem.h"

class ScriptModule : public IModule {
    std::unique_ptr<MainScript> m_mainScript;
    std::unique_ptr<ScriptSystem> m_scriptSystem;

    ModuleManager *m_moduleManager;

public:
    ScriptModule(ModuleManager *mm, const std::string &scriptPath = "../assets/scripts/main.as");

    bool Initialize() override;
    void Update(float deltaTime) override;
    void Shutdown() override;

    const char *GetName() const override { return "ScriptModule"; }
    int GetPriority() const override     { return 80; }
    bool IsRequired() const override     { return true; };
};
