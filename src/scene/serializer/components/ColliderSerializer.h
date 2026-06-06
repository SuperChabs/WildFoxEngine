#pragma once

#include <nlohmann/json.hpp>
#include <glm/glm.hpp>

#include <variant>

#include "ECS/World.h"
#include "ECS/components/Components.h"
#include "scene/serializer/components/IComponentSerializer.h"

using json = nlohmann::json;

class ColliderSerializer : public IComponentSerializer {
public:
    json Serialize(ECSWorld *world, entt::entity entity) override {
        if (!world->HasComponent<ColliderComponent>(entity))
            return json::object();

        auto &collider = world->GetComponent<ColliderComponent>(entity);

        if (auto *aabb = std::get_if<AABB>(&collider.shape)) {
            return json{
                {"type", "AABB"},
                {"min", {aabb->min.x, aabb->min.y, aabb->min.z}},
                {"max", {aabb->max.x, aabb->max.y, aabb->max.z}},
                {"isTrigger", collider.isTrigger}
            };
        }

        if (auto *sphere = std::get_if<Sphere>(&collider.shape)) {
            return json{
                {"type", "Sphere"},
                {"centre", {sphere->centre.x, sphere->centre.y, sphere->centre.z}},
                {"radius", sphere->radius},
                {"isTrigger", collider.isTrigger}
            };
        }

        return json::object();
    }

    void Deserialize(ECSWorld *world, entt::entity entity, const json &data) override {
        if (!data.contains("type"))
            return;

        bool isTrigger;

        std::string type = data.value("type", "");

        if (data.contains("isTrigger") && data["isTrigger"].is_boolean())
            isTrigger = data["isTrigger"].get<bool>();
        else
            isTrigger = false;

        if (type == "AABB") {
            AABB aabb;
            aabb.min = Vec3FromJson(data.value("min", json::array({0.0f, 0.0f, 0.0f})));
            aabb.max = Vec3FromJson(data.value("max", json::array({0.0f, 0.0f, 0.0f})));
            world->AddComponent<ColliderComponent>(entity, aabb, isTrigger);
        } else if (type == "Sphere") {
            Sphere sphere;
            sphere.centre = Vec3FromJson(data.value("centre", json::array({0.0f, 0.0f, 0.0f})));
            sphere.radius = data.value("radius", 0.0f);
            world->AddComponent<ColliderComponent>(entity, sphere, isTrigger);
        }
    }

    bool CanSerialize(ECSWorld *world, entt::entity entity) const override {
        return world->HasComponent<ColliderComponent>(entity);
    }
};