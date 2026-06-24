#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <any>
#include <functional>

class EventBus {
    using Callback = std::function<void(const std::any &)>;
    std::unordered_map<std::string, std::vector<Callback> > m_Subscribers;

public:
    void RegisterEvent(const std::string &name);

    void Subscribe(const std::string &name, Callback callback);

    void Publish(const std::string &name, const std::any &payload = {});
};

EventBus &GetEventBus();
