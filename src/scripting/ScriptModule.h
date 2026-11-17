#pragma once

#include <memory>

#include "core/ModuleManager.h"
#include "core/IModule.h"
#include "ECS/systems/ScriptSystem.h"

class ScriptModule : public IModule {
    std::unique_ptr<ScriptSystem> m_scriptSystem;

    ModuleManager *m_moduleManager;

public:
    ScriptModule(ModuleManager *mm, const std::string &scriptPath = "../assets/scripts/main.as");

    bool OnInitialize() override;

    void Update(float deltaTime) override;

    void Shutdown() override;

    const char *GetName() const override { return "ScriptModule"; }
    bool IsRequired() const override { return true; }

    ScriptSystem *GetScriptSystem() const { return m_scriptSystem.get(); }
};