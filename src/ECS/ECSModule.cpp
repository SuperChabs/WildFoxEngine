#include "ECSModule.h"
#include <string>
#include "core/logging/Logger.h"

bool ECSModule::OnInitialize() {
    try {
        ecsWorld = std::make_unique<ECSWorld>();

        Logger::Log(LogLevel::INFO, "Successfully initialized ECS module");

        return true;
    } catch (std::exception &e) {
        Logger::Log(LogLevel::ERROR,
                    "Exception creating ecs module: " + std::string(e.what()));

        return false;
    }
}

void ECSModule::Update(float deltaTime) {
}

void ECSModule::Shutdown() {
    ecsWorld.reset();
}

const char *ECSModule::GetName() const {
    return "ECS";
}

bool ECSModule::IsRequired() const {
    return true;
}

ECSWorld *ECSModule::GetECS() const {
    return ecsWorld.get();
}
