#pragma once

#include <string>
#include <map>
#include <memory>
#include <vector>

#include "core/IModule.h"
#include "core/Time.h"

class ModuleManager {
    std::vector<std::unique_ptr<IModule> > modules;
    std::map<std::string, IModule *> moduleMap;

public:
    template<typename T, typename... Args>
    T *RegisterModule(Args &&... args) {
        auto module = std::make_unique<T>(std::forward<Args>(args)...);
        T *ptr = module.get();
        moduleMap[module->GetName()] = ptr;
        modules.push_back(std::move(module));
        return ptr;
    }

    void InitializeAll();

    void UpdateAll(float deltaTime);

    void ShutdownAll();

    template<typename T>
    T *GetModule(const std::string &name) {
        auto it = moduleMap.find(name);
        if (it != moduleMap.end())
            return dynamic_cast<T *>(it->second);
        return nullptr;
    }

    bool IsModuleActive(const std::string &name) const;
};