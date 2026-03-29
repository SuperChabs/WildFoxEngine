module;

#include <ext/entt.hpp>
#include <ext/json.hpp>

export module WFE.Scene.Serializer.Component.CameraSerializer;

import WFE.ECS.ECSWorld;
import WFE.ECS.Components;
import WFE.Scene.Serializer.Component.IComponentSerializer;

using json = nlohmann::json;

export class CameraSerializer : public IComponentSerializer
{
public:
    json Serialize(ECSWorld* world, entt::entity entity) override
    {
        if (!world->HasComponent<CameraComponent>(entity))
            return json::object();

        auto& cam = world->GetComponent<CameraComponent>(entity);
        json camData{
            {"fov", cam.fov},
            {"nearPlane", cam.nearPlane},
            {"farPlane", cam.farPlane},
            {"movementSpeed", cam.movementSpeed},
            {"mouseSensitivity", cam.mouseSensitivity},
            {"isMainCamera", cam.isMainCamera},
            {"isActive", cam.isActive}
        };

        if (world->HasComponent<CameraOrientationComponent>(entity))
        {
            auto& orient = world->GetComponent<CameraOrientationComponent>(entity);
            camData["orientation"] = {
                {"yaw", orient.yaw},
                {"pitch", orient.pitch}
            };
        }

        return camData;
    }

    void Deserialize(ECSWorld* world, entt::entity entity, const json& data) override
    {
        auto& camComp = world->AddComponent<CameraComponent>(entity);
        camComp.fov = data.value("fov", 55.0f);
        camComp.nearPlane = data.value("nearPlane", 0.1f);
        camComp.farPlane = data.value("farPlane", 1000.0f);
        camComp.movementSpeed = data.value("movementSpeed", 3.5f);
        camComp.mouseSensitivity = data.value("mouseSensitivity", 0.1f);
        camComp.isMainCamera = data.value("isMainCamera", false);
        camComp.isActive = data.value("isActive", true);

        if (data.contains("orientation"))
        {
            auto& orient = data["orientation"];
            auto& orientComp = world->AddComponent<CameraOrientationComponent>(entity);
            orientComp.yaw = orient.value("yaw", -95.0f);
            orientComp.pitch = orient.value("pitch", 0.0f);
        }
    }

    bool CanSerialize(ECSWorld* world, entt::entity entity) const override
    {
        return world->HasComponent<CameraComponent>(entity);
    }
};

export class CameraTypeSerializer : public IComponentSerializer
{
public:
    json Serialize(ECSWorld* world, entt::entity entity) override
    {
        if (!world->HasComponent<CameraTypeComponent>(entity))
            return json::object();

        auto& camType = world->GetComponent<CameraTypeComponent>(entity);
        return (camType.type == CameraTypeComponent::Type::GAME) ? "GAME" : "EDITOR";
    }

    void Deserialize(ECSWorld* world, entt::entity entity, const json& data) override
    {
        std::string typeStr = data.is_string() ? data.get<std::string>() : "EDITOR";
        auto& camType = world->AddComponent<CameraTypeComponent>(entity);
        camType.type = (typeStr == "GAME") ? CameraTypeComponent::Type::GAME : CameraTypeComponent::Type::EDITOR;
    }

    bool CanSerialize(ECSWorld* world, entt::entity entity) const override
    {
        return world->HasComponent<CameraTypeComponent>(entity);
    }
};