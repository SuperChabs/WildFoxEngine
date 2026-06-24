#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>

#include <entt/entt.hpp>
#include <nlohmann/json.hpp>

#include "ECS/World.h"
#include "scene/serializer/components/ComponentSerializers.h"

using json = nlohmann::json;

class SerializerRegistry {
private:
    std::unordered_map<std::string, std::unique_ptr<IComponentSerializer> > serializers;

public:
    SerializerRegistry();

    void RegisterSerializer(const std::string &name, std::unique_ptr<IComponentSerializer> serializer);

    template<typename T>
    void RegisterSerializer(const std::string &name) {
        serializers[name] = std::make_unique<T>();
    }

    IComponentSerializer *GetSerializer(const std::string &name) const;

    json SerializeAllComponents(ECSWorld *world, entt::entity entity);

    void DeserializeAllComponents(ECSWorld *world, entt::entity entity, const json &entityData);

    std::vector<std::string> GetSerializerNames() const;

private:
    void RegisterDefaultSerializers();
};
