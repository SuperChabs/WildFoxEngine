#pragma once

#include <entt/entt.hpp>
#include <nlohmann/json.hpp>
#include <glm/glm.hpp>

#include "ECS/World.h"
#include "ECS/components/Components.h"
#include "scene/serializer/components/IComponentSerializer.h"

using json = nlohmann::json;

static glm::vec3 Vec3FromJson(const json &value) {
    if (!value.is_array() || value.size() < 3)
        return glm::vec3(0.0f);

    return glm::vec3(value[0].get<float>(), value[1].get<float>(), value[2].get<float>());
}

class RigidBodySerializer : public IComponentSerializer {
public:
    json Serialize(ECSWorld *world, entt::entity entity) override;


    void Deserialize(ECSWorld *world, entt::entity entity, const json &data) override;


    bool CanSerialize(ECSWorld *world, entt::entity entity) const override;
};
