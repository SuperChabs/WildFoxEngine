#include "ModuleManager.h"

#include <ranges>

ModuleManager::~ModuleManager() {
    for (auto &module: std::views::reverse(modules)) {
        delete module;
    }
    modules.clear();
}

void ModuleManager::UpdateAll(float deltaTime) const {
    for (auto &module: modules)
        if (module->IsInitialized() && module->IsShouldUpdate())
            module->Update(deltaTime);
}

void ModuleManager::ShutdownAll() const {
    for (auto &module: std::views::reverse(modules))
        module->Shutdown();
}