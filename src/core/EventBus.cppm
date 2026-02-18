module;

#include <string>
#include <any>
#include <vector>
#include <functional>
#include <unordered_map>

export module WFE.Core.EventBus;

export class EventBus
{
    using Callback = std::function<void(const std::any&)>;
    std::unordered_map<std::string, std::vector<Callback>> m_Subscribers;

public:
    void RegisterEvent(const std::string& name)
    {
        if (m_Subscribers.find(name) == m_Subscribers.end())
            m_Subscribers.emplace(name, std::vector<Callback>());
    }

    void Subscribe(const std::string& name, Callback callback)
    {
        RegisterEvent(name);
        m_Subscribers[name].push_back(std::move(callback));
    }

    void Publish(const std::string& name, const std::any& payload = {})
    {
        auto it = m_Subscribers.find(name);
        if (it == m_Subscribers.end())
            return;

        for (auto& cb : it->second)
        {
            try { cb(payload); }
            catch (...) { /* swallow exceptions in event callbacks */ }
        }
    }
};

export EventBus& GetEventBus()
{
    static EventBus bus;
    return bus;
}