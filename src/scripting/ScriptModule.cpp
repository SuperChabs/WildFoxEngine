#include "ScriptModule.h"

#include "core/logging/Logger.h"
#include "ECS/ECSModule.h"
#include "scripting/ASBindings.h"
#include "core/CoreModule.h"
#include "scene/SceneModule.h"

ScriptModule::ScriptModule(ModuleManager *mm, const std::string &scriptPath)
    : m_moduleManager(mm) {
    m_scriptSystem = std::make_unique<ScriptSystem>();
}

bool ScriptModule::OnInitialize() {
    try {
        Logger::Log(LogLevel::INFO, "Initializing AngelScript...");

        try {
            InitAS(m_moduleManager->GetModule<ECSModule>()->GetECS(),
                   m_moduleManager->GetModule<CoreModule>()->GetInput(), nullptr);
            Logger::Log(LogLevel::INFO, "AngelScript initialized successfully");
        } catch (const std::exception &e) {
            Logger::Log(LogLevel::ERROR, "Failed to initialize AngelScript: " + std::string(e.what()));

            return false;
        }


        Logger::Log(LogLevel::INFO, "Initialized ScriptModule successfully");

        return true;
    } catch (std::exception &e) {
        Logger::Log(LogLevel::ERROR, "Exception creating ui module: " + std::string(e.what()));
        return false;
    }
}

void ScriptModule::Update(const float deltaTime) {
    ScriptSystem::Update(*m_moduleManager->GetModule<ECSModule>()->GetECS(), deltaTime);
}

void ScriptModule::Shutdown() {
}