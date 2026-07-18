#include "ScriptModule.h"

#include "core/logging/Logger.h"
#include "ECS/ECSModule.h"
#include "scripting/ASBindings.h"
#include "core/CoreModule.h"

ScriptModule::ScriptModule(ModuleManager *mm, const std::string &scriptPath)
    : m_moduleManager(mm) {
    m_mainScript = std::make_unique<MainScript>(scriptPath);
    m_scriptSystem = std::make_unique<ScriptSystem>();
}

bool ScriptModule::Initialize() {
    try {
        Logger::Log(LogLevel::INFO, "Initializing AngelScript...");

        try {
            InitAS(m_moduleManager->GetModule<ECSModule>("ECS")->GetECS(),
                m_moduleManager->GetModule<CoreModule>("Core")->GetInput(), nullptr);
            Logger::Log(LogLevel::INFO, "AngelScript initialized successfully");
        } catch (const std::exception &e) {
            Logger::Log(LogLevel::ERROR, "Failed to initialize AngelScript: " + std::string(e.what()));

            return false;
        }

        m_mainScript->Start();
        if (!m_mainScript) {
            Logger::Log(LogLevel::ERROR, "Failed to initialize MainScript");
            return false;
        }

        Logger::Log(LogLevel::INFO, "Initialized ScriptModule successfully");

        isInitialized = true;
        return true;
    } catch (std::exception &e) {
        Logger::Log(LogLevel::ERROR, "Exception creating ui module: " + std::string(e.what()));
        return false;
    }
}

void ScriptModule::Update(float deltaTime) {
    m_mainScript->Update(deltaTime);
    m_scriptSystem->Update(*m_moduleManager->GetModule<ECSModule>("ECS")->GetECS(), deltaTime);
}

void ScriptModule::Shutdown() {
    m_mainScript->Stop();
}
