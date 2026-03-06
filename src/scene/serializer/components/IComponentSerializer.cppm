module;

#include <entt/entt.hpp>
#include <nlohmann/json.hpp>

export module WFE.Scene.Serializer.Component.IComponentSerializer;

import WFE.ECS.ECSWorld;

using json = nlohmann::json;

export class IComponentSerializer
{
public:
    virtual ~IComponentSerializer() = default;
    virtual json Serialize(ECSWorld* world, entt::entity entity) = 0;
    virtual void Deserialize(ECSWorld* world, entt::entity entity, const json& data) = 0;
    virtual bool CanSerialize(ECSWorld* world, entt::entity entity) const = 0;
};