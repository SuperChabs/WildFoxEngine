#include "ColliderSerializer.h"

glm::vec3 Vec3FromJson(const json &value) {
    if (!value.is_array() || value.size() < 3)
        return glm::vec3(0.0f);

    return {value[0].get<float>(), value[1].get<float>(), value[2].get<float>()};
}

json ColliderSerializer::Serialize(ECSWorld *world, entt::entity entity) {
    if (!world->HasComponent<c_Collider>(entity))
        return json::object();

    auto &collider = world->GetComponent<c_Collider>(entity);

    if (auto *sphere = std::get_if<Sphere>(&collider.shape)) {
        return json{
            {"type", "Sphere"},
            {"centre", {sphere->m_center.x, sphere->m_center.y, sphere->m_center.z}},
            {"radius", sphere->m_radius},
            {"isTrigger", collider.isTrigger}
        };
    }

    if (auto *hull = std::get_if<ConvexHull>(&collider.shape)) {
        json points = json::array();
        for (const glm::vec3 &point : hull->m_points)
            points.push_back({point.x, point.y, point.z});

        return json{
            {"type", "ConvexHull"},
            {"points", std::move(points)},
            {"isTrigger", collider.isTrigger}
        };
    }

    return json::object();
}

entt::entity ColliderSerializer::Deserialize(DeserializeContext &dcx, entt::entity entity, const json &data) {
    if (!data.contains("type"))
        return entt::null;

    bool isTrigger;

    std::string type = data.value("type", "");

    if (data.contains("isTrigger") && data["isTrigger"].is_boolean())
        isTrigger = data["isTrigger"].get<bool>();
    else
        isTrigger = false;

    if (type == "Sphere") {
        Sphere sphere;
        sphere.m_center = Vec3FromJson(data.value("centre", json::array({0.0f, 0.0f, 0.0f})));
        sphere.m_radius = data.value("radius", 0.0f);
        dcx.world->AddComponent<c_Collider>(entity, sphere, isTrigger);
    } else if (type == "ConvexHull" && data.contains("points") && data["points"].is_array()) {
        std::vector<glm::vec3> points;
        for (const auto &point : data["points"])
            if (point.is_array() && point.size() >= 3)
                points.push_back(Vec3FromJson(point));

        c_Collider collider;
        if (collider.GenerateConvexHull(points)) {
            collider.isTrigger = isTrigger;
            dcx.world->AddComponent<c_Collider>(entity, std::move(collider));
        }
    }

    return entity;
}

bool ColliderSerializer::CanSerialize(ECSWorld *world, entt::entity entity) const {
    return world->HasComponent<c_Collider>(entity);
}