module;

#include <memory>
#include <vector>
#include <map>
#include <algorithm>
#include <utility>

export module WFE.Core.ModuleManager;

import WFE.Core.Logger;
import WFE.Core.IModule;
import WFE.Core.Time;

export class ModuleManager
{
    std::vector<std::unique_ptr<IModule>> modules;
    std::map<std::string, IModule*> moduleMap;

public:
    template<typename T, typename... Args>
    T* RegisterModule(Args&&... args)
    {
        auto module = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = module.get();
        moduleMap[module->GetName()] = ptr;
        modules.push_back(std::move(module));
        return ptr;
    }

    void InitializeAll()
    {
        std::sort(modules.begin(), modules.end(), [](auto& a, auto& b)
        {
            return a->GetPriority() < b->GetPriority();
        });

        for (auto& module : modules)
        {
            Logger::Log(LogLevel::INFO, std::string("Initializing ") + module->GetName() + "...");

            if(!module->Initialize())
            {
                if (module->IsRequired())
                {
                    Logger::Log(LogLevel::CRITICAL, 
                        std::string("Required module failed: ") + module->GetName());
                    throw std::runtime_error("Critical module init failed");
                }
                else 
                {
                    Logger::Log(LogLevel::WARNING, 
                        std::string("Optional module failed: ") + module->GetName());
                }
            }
        }
    }

    void UpdateAll(float deltaTime)
    {
        std::sort(modules.begin(), modules.end(), [](auto& a, auto& b)
        {
            return a->GetPriority() < b->GetPriority();
        });

        for (auto& module : modules)
            module->Update(deltaTime);
    }

    void ShutdownAll()
    {
        std::sort(modules.begin(), modules.end(), [](auto& a, auto& b)
        {
            return a->GetPriority() < b->GetPriority();
        });

        for (auto& module : modules)
            module->Shutdown();
    }

    template<typename T>
    T* GetModule(const std::string& name)
    {
        auto it = moduleMap.find(name);
        if (it != moduleMap.end())
            return dynamic_cast<T*>(it->second);
        return nullptr;
    }

    bool IsModuleActive(const std::string& name) const
    {
        auto it = moduleMap.find(name);
        return it != moduleMap.end();
    }
};