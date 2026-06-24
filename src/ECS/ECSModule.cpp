#include "ECSModule.h"
#include <string>
#include "core/logging/Logger.h"

bool ECSModule::Initialize() {
    try {
        ecsWorld = std::make_unique<ECSWorld>();

        Logger::Log(LogLevel::INFO, "Successfully initialized ECS module");

        isInitialized = true;

        return true;
    } catch (std::exception &e) {
        Logger::Log(LogLevel::ERROR,
                    "Exception creating ecs module: " + std::string(e.what()));

        isInitialized = false;

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

int ECSModule::GetPriority() const {
    return 10;
}

bool ECSModule::IsRequired() const {
    return true;
}

ECSWorld *ECSModule::GetECS() {
    return ecsWorld.get();
}