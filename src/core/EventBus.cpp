#include "EventBus.h"
#include <functional>

void EventBus::RegisterEvent(const EventID &name) {
    if (m_Subscribers.find(name) == m_Subscribers.end())
        m_Subscribers.emplace(name, std::vector<std::pair<SubscriberID, Callback>>());
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
            auto inner = std::find_if(it->second.begin(), it->second.end(),
                [id](const auto &pair) { return pair.first == id; });

            if (inner != it->second.end())
                it->second.erase(inner);
        }
    } else {
        auto it = m_Subscribers.find(name);
        if (it != m_Subscribers.end())
            m_PendingUnsubscribes.push_back({name, id});
    }
}

void EventBus::Publish(const EventID &name, const std::any &payload) {
    auto it = m_Subscribers.find(name);
    if (it == m_Subscribers.end())
        return;

    m_IsPublishing = true;

    for (auto &[id, cb]: it->second) {
        try {
            cb(payload);
        } catch (std::exception &e) {
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
