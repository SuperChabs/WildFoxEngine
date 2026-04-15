module;

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <entt/entt.hpp>
#include <nlohmann/json.hpp>

export module WFE.Scene.Serializer.Component.TransformSerializer;

import WFE.ECS.ECSWorld;
import WFE.ECS.Components;
import WFE.Scene.Serializer.Component.IComponentSerializer;

using json = nlohmann::json;

export class TransformSerializer : public IComponentSerializer
{
public:
    json Serialize(ECSWorld* world, entt::entity entity) override
    {
        if (!world->HasComponent<TransformComponent>(entity))
            return json::object();

        auto& t = world->GetComponent<TransformComponent>(entity);

        glm::quat normRot = glm::normalize(t.rotation);
        glm::vec3 euler = glm::degrees(glm::eulerAngles(normRot));

        return json{
            {"position", {t.position.x, t.position.y, t.position.z}},
            {"rotation", {euler.x, euler.y, euler.z}},
            {"scale",    {t.scale.x,    t.scale.y,    t.scale.z}}
        };
    }

    void Deserialize(ECSWorld* world, entt::entity entity, const json& data) override
    {
        if (!data.contains("position") || !data.contains("rotation"))
            return;

        glm::vec3 pos = {data["position"][0], data["position"][1], data["position"][2]};
        glm::vec3 eulerDeg(data["rotation"][0], data["rotation"][1], data["rotation"][2]);
        glm::quat rot = glm::quat(glm::radians(eulerDeg));
        glm::vec3 scl = data.contains("scale")
            ? glm::vec3{data["scale"][0], data["scale"][1], data["scale"][2]}
            : glm::vec3{1.0f};

        world->AddComponent<TransformComponent>(entity, pos, rot, scl);
    }

    bool CanSerialize(ECSWorld* world, entt::entity entity) const override
    {
        return world->HasComponent<TransformComponent>(entity);
    }
};