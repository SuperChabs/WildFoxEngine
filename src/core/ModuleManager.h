#pragma once

#include <memory>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "core/IModule.h"

class ModuleManager {
    std::vector<IModule*> modules;
    std::unordered_map<std::type_index, IModule *> moduleMap;

public:
    ~ModuleManager();

    template<typename T, typename... Args>
    T *RegisterModule(Args &&... args) {
        T *ptr  = new T(std::forward<Args>(args)...);
        moduleMap[typeid(T)] = ptr;
        modules.push_back(ptr);
        return ptr;
    }

    void UpdateAll(float deltaTime) const;

    /// @brief Shuts modules down in reverse registration order
    void ShutdownAll() const;

    template<typename T>
    T *GetModule() const {
        const auto it = moduleMap.find(typeid(T));
        if (it != moduleMap.end())
            return static_cast<T *>(it->second);
        return nullptr;
    }
};
