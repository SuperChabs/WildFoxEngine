#include "ModuleManager.h"

#include <ranges>

ModuleManager::~ModuleManager() {
    for (const auto &module: std::views::reverse(modules)) {
        delete module;
    }
    modules.clear();
}

void ModuleManager::UpdateAll(const float deltaTime) {
    for (const auto &module: modules)
        if (module->IsInitialized() && module->IsShouldUpdate())
            module->Update(deltaTime);
}

void ModuleManager::ShutdownAll() {
    for (const auto &module: std::views::reverse(modules))
        module->Shutdown();
}