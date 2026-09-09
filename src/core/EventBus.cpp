#include "EventBus.h"
#include <functional>
#include "logging/Logger.h"

void EventBus::RegisterEvent(const EventID &name) {
    if (!m_Subscribers.contains(name))
        m_Subscribers.emplace(name, std::vector<std::pair<SubscriberID, Callback> >());
}

SubscriberID EventBus::Subscribe(const EventID &name, Callback callback) {
    m_NextID++;

    RegisterEvent(name);

    m_Subscribers[name].emplace_back(m_NextID, std::move(callback));

    return m_NextID;
}

void EventBus::Unsubscribe(const EventID &name, SubscriberID id) {
    if (!m_IsPublishing) {
        auto it = m_Subscribers.find(name);
        if (it != m_Subscribers.end() && it->first == name) {
            const auto inner = std::ranges::find_if(it->second,
                                                    [id](const auto &pair) { return pair.first == id; });

            if (inner != it->second.end())
                it->second.erase(inner);
        }
    } else {
        auto it = m_Subscribers.find(name);
        if (it != m_Subscribers.end())
            m_PendingUnsubscribes.emplace_back(name, id);
    }
}

void EventBus::Publish(const EventID &name, const std::any &payload) {
    const auto it = m_Subscribers.find(name);
    if (it == m_Subscribers.end())
        return;

    m_IsPublishing = true;

    for (auto &cb: it->second | std::views::values) {
        try {
            cb(payload);
        } catch (std::exception &e) {
            Logger::Log(LogLevel::ERROR, e.what());
        }
    }

    m_IsPublishing = false;

    for (auto &[eID, sID]: m_PendingUnsubscribes) {
        Unsubscribe(eID, sID);
    }
    m_PendingUnsubscribes.clear();
}

EventBus &GetEventBus() {
    static EventBus eventbus;
    return eventbus;
}