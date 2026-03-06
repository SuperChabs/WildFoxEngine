module;

#include <entt/entt.hpp>
#include <nlohmann/json.hpp>

export module WFE.Scene.Serializer.Component.LightSerializer;

import WFE.ECS.ECSWorld;
import WFE.ECS.Components;
import WFE.Scene.Serializer.Component.IComponentSerializer;
import WFE.Scene.Light;

using json = nlohmann::json;

export class LightSerializer : public IComponentSerializer
{
public:
    json Serialize(ECSWorld* world, entt::entity entity) override
    {
        if (!world->HasComponent<LightComponent>(entity))
            return json::object();

        auto& light = world->GetComponent<LightComponent>(entity);
        return json{
            {"type", static_cast<int>(light.type)},
            {"position", {light.position.x, light.position.y, light.position.z}},
            {"direction", {light.direction.x, light.direction.y, light.direction.z}},
            {"ambient", {light.ambient.x, light.ambient.y, light.ambient.z}},
            {"diffuse", {light.diffuse.x, light.diffuse.y, light.diffuse.z}},
            {"specular", {light.specular.x, light.specular.y, light.specular.z}},
            {"intensity", light.intensity},
            {"isActive", light.isActive},
            {"castShadows", light.castShadows},
            {"constant", light.constant},
            {"linear", light.linear},
            {"quadratic", light.quadratic},
            {"radius", light.radius},
            {"innerCutoff", light.innerCutoff},
            {"outerCutoff", light.outerCutoff}
        };
    }

    void Deserialize(ECSWorld* world, entt::entity entity, const json& data) override
    {
        LightType type = static_cast<LightType>(data.value("type", 0));
        auto& lightComp = world->AddComponent<LightComponent>(entity, type);

        if (data.contains("position"))
            lightComp.position = {data["position"][0], data["position"][1], data["position"][2]};
        if (data.contains("direction"))
            lightComp.direction = {data["direction"][0], data["direction"][1], data["direction"][2]};
        if (data.contains("ambient"))
            lightComp.ambient = {data["ambient"][0], data["ambient"][1], data["ambient"][2]};
        if (data.contains("diffuse"))
            lightComp.diffuse = {data["diffuse"][0], data["diffuse"][1], data["diffuse"][2]};
        if (data.contains("specular"))
            lightComp.specular = {data["specular"][0], data["specular"][1], data["specular"][2]};

        lightComp.intensity = data.value("intensity", 1.0f);
        lightComp.isActive = data.value("isActive", true);
        lightComp.castShadows = data.value("castShadows", true);
        lightComp.constant = data.value("constant", 1.0f);
        lightComp.linear = data.value("linear", 0.09f);
        lightComp.quadratic = data.value("quadratic", 0.032f);
        lightComp.radius = data.value("radius", 50.0f);
        lightComp.innerCutoff = data.value("innerCutoff", 12.5f);
        lightComp.outerCutoff = data.value("outerCutoff", 17.5f);
    }

    bool CanSerialize(ECSWorld* world, entt::entity entity) const override
    {
        return world->HasComponent<LightComponent>(entity);
    }
};