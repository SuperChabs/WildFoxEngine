#include "EventBus.h"
#include <functional>

void EventBus::RegisterEvent(const std::string &name) {
    if (m_Subscribers.find(name) == m_Subscribers.end())
        m_Subscribers.emplace(name, std::vector<Callback>());
}

void EventBus::Subscribe(const std::string &name, Callback callback) {
    RegisterEvent(name);
    m_Subscribers[name].push_back(std::move(callback));
}

void EventBus::Publish(const std::string &name, const std::any &payload) {
    auto it = m_Subscribers.find(name);
    if (it == m_Subscribers.end())
        return;

    for (auto &cb: it->second) {
        try { cb(payload); } catch (...) {
            /* swallow exceptions in event callbacks */
        }
    }
}

EventBus &GetEventBus() {
    static EventBus eventbus;
    return eventbus;
}
