#include "ColliderPanel.h"

#include <algorithm>
#include <cmath>
#include <functional>
#include <unordered_set>

#include "core/logging/Logger.h"

namespace {
    bool CollectMeshVertices(ECSWorld *ecs, const entt::entity root, std::vector<glm::vec3> &points) {
        const glm::mat4 rootTransform = ecs->GetGlobalTransform(root);
        const float determinant = glm::determinant(rootTransform);
        if (!std::isfinite(determinant) || std::abs(determinant) < 1e-8f)
            return false;

        const glm::mat4 worldToRoot = glm::inverse(rootTransform);
        std::unordered_set<entt::entity> visited;
        std::function<void(entt::entity)> collect = [&](const entt::entity current) {
            if (!ecs->IsValid(current) || !visited.insert(current).second)
                return;

            if (ecs->HasComponent<MeshComponent>(current)) {
                const auto &meshComponent = ecs->GetComponent<MeshComponent>(current);
                if (meshComponent.mesh) {
                    const glm::mat4 meshToRoot = worldToRoot * ecs->GetGlobalTransform(current);
                    for (const Vertex &vertex : meshComponent.mesh->GetVertices()) {
                        const glm::vec4 localPoint = meshToRoot * glm::vec4(vertex.Position, 1.0f);
                        points.emplace_back(localPoint);
                    }
                }
            }

            for (const entt::entity child : ecs->GetChildren(current))
                collect(child);
        };

        collect(root);
        return !points.empty();
    }
}

void ColliderPanel::Render(ECSWorld *ecs, entt::entity entity) {
    static int maxHullVertices = 256;

    if (!ecs->HasComponent<ColliderComponent>(entity)) return;
    if (!ImGui::CollapsingHeader("Collider", ImGuiTreeNodeFlags_DefaultOpen)) return;

    auto &t = ecs->GetComponent<TransformComponent>(entity);
    auto &c = ecs->GetComponent<ColliderComponent>(entity);

    ImGui::Text("Hitbox");

    if (std::holds_alternative<Sphere>(c.shape)) {
        Sphere &sphere = std::get<Sphere>(c.shape);
        ImGui::DragFloat("Radius", &sphere.m_radius, 0.01f);
        ImGui::DragFloat3("Center", &sphere.m_center[0], 0.01f);

        if (ImGui::Button("Set hitbox scale to entity scale")) {
            Sphere sphere{};
            sphere.m_radius = 1.0f;
            c.shape = sphere;
        }
    } else if (std::holds_alternative<ConvexHull>(c.shape)) {
        ImGui::Text("Convex hull: %zu vertices", std::get<ConvexHull>(c.shape).m_points.size());
    } else if (std::holds_alternative<Box>(c.shape)) {
        ImGui::Text("Box collider");
    }

    ImGui::InputInt("Max hull vertices", &maxHullVertices);
    maxHullVertices = std::clamp(maxHullVertices, 4, 1024);
    if (ImGui::Button("Generate convex hull from mesh")) {
        std::vector<glm::vec3> points;
        if (!CollectMeshVertices(ecs, entity, points)) {
            Logger::Log(LogLevel::ERROR, "Cannot generate collider: mesh hierarchy is empty or has a singular transform");
        } else if (!c.GenerateConvexHull(points, maxHullVertices)) {
            Logger::Log(LogLevel::ERROR, "Cannot generate convex collider from mesh: at least four non-coplanar points are required");
        }
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Text("Is Trigger:");
    if (ImGui::Button(c.isTrigger ? "Yes" : "No"))
        c.isTrigger = !c.isTrigger;
}