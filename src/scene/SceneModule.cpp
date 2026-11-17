#include "SceneModule.h"
#include <string>
#include "core/logging/Logger.h"

SceneModule::SceneModule(ECSWorld *world)
    : ecs(world) {
}

bool SceneModule::OnInitialize() {
    try {
        m_Serializer = std::make_unique<SceneSerializer>(ecs);
        m_SceneManager = std::make_unique<SceneManager>(ecs);

        Logger::Log(LogLevel::INFO, "SceneModule initialized");

        return true;
    } catch (const std::exception &e) {
        Logger::Log(LogLevel::ERROR, "SceneModule initialization failed: " + std::string(e.what()));

        return false;
    }
}

void SceneModule::Update(float deltaTime) {
}

void SceneModule::Shutdown() {
    m_Serializer.reset();
    m_SceneManager.reset();
}

const char *SceneModule::GetName() const {
    return "Scene";
}

bool SceneModule::IsRequired() const {
    return true;
}

SceneSerializer *SceneModule::GetSceneSerializer() {
    return m_Serializer.get();
}

SceneManager *SceneModule::GetSceneManager() {
    return m_SceneManager.get();
}