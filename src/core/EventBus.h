#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <any>
#include <functional>

using Callback = std::function<void(const std::any &)>;
using EventID = std::string;
using SubscriberID = size_t;

class EventBus {
    std::unordered_map<EventID, std::vector<std::pair<SubscriberID, Callback>>> m_Subscribers;
    std::vector<std::pair<EventID, SubscriberID>> m_PendingUnsubscribes;

    bool m_IsPublishing = false;
    SubscriberID m_NextID = -1;

public:
    void RegisterEvent(const EventID &name);

    SubscriberID Subscribe(const EventID &name, Callback callback);
    void Unsubscribe(const EventID &name, SubscriberID id);

    void Publish(const EventID &name, const std::any &payload = {});
};

EventBus &GetEventBus();
