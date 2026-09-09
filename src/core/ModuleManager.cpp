#include "ModuleManager.h"
#include "core/logging/Logger.h"

void ModuleManager::InitializeAll() {
    std::ranges::sort(modules, [](auto &a, auto &b) {
        return a->GetPriority() < b->GetPriority();
    });

    for (auto &module: modules) {
        Logger::Log(LogLevel::INFO, std::string("Initializing ") + module->GetName() + "...");

        if (!module->Initialize()) {
            if (module->IsRequired()) {
                Logger::Log(LogLevel::CRITICAL,
                            std::string("Required module failed: ") + module->GetName());
                throw std::runtime_error("Critical module init failed");
            }
            Logger::Log(LogLevel::WARNING,
                        std::string("Optional module failed: ") + module->GetName());
        }
    }
}

void ModuleManager::UpdateAll(float deltaTime) const {
    for (auto &module: modules)
        module->Update(deltaTime);
}

void ModuleManager::ShutdownAll() const {
    for (auto &module: modules)
        module->Shutdown();
}

bool ModuleManager::IsModuleActive(const std::string &name) const {
    const auto it = moduleMap.find(name);
    return it != moduleMap.end();
}