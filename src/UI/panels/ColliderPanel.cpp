#include "ColliderPanel.h"

void ColliderPanel::Render(ECSWorld *ecs, entt::entity entity) {
    if (!ecs->HasComponent<ColliderComponent>(entity)) return;
    if (!ImGui::CollapsingHeader("Collider", ImGuiTreeNodeFlags_DefaultOpen)) return;

    auto &t = ecs->GetComponent<TransformComponent>(entity);
    auto &c = ecs->GetComponent<ColliderComponent>(entity);

    ImGui::Text("Hitbox");

    if (std::holds_alternative<AABB>(c.shape)) {
        AABB &aabb = std::get<AABB>(c.shape);
        ImGui::DragFloat3("Min##aabb", &aabb.min[0], 0.01f);
        ImGui::DragFloat3("Max##aabb", &aabb.max[0], 0.01f);

        if (ImGui::Button("Set hitbox scale to entity scale")) {
            AABB aabb;
            aabb.min = -t.scale;
            aabb.max = t.scale;
            c.shape = aabb;
        }
    }
    else if (std:: holds_alternative<Sphere>(c.shape)) {
        Sphere &sphere = std::get<Sphere>(c.shape);
        ImGui::DragFloat("Radius", &sphere.m_radius, 0.01f);
        ImGui::DragFloat3("Center", &sphere.m_center[0], 0.01f);

        if (ImGui::Button("Set hitbox scale to entity scale")) {
            Sphere sphere;
            sphere.m_radius = (t.scale.x + t.scale.y + t.scale.z) / 3.0f;
            sphere.m_center = t.position;
            c.shape = sphere;
        }
    }

    ImGui::Text("Change Hitbox type");
    if (std::holds_alternative<Sphere>(c.shape)) {
        if (ImGui::Button("Set hitbox as AABB")) {
            AABB aabb = { glm::vec3{-1.0f, -1.0f, -1.0f}, glm::vec3{1.0f, 1.0f, 1.0f} };
            c.shape = aabb;
        }
    }
    else if (std::holds_alternative<AABB>(c.shape)) {
        if (ImGui::Button("Set hitbox as Sphere")) {
            Sphere sphere = { glm::vec3{1.0f} ,1.0f };
            c.shape = sphere;
        }
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Text("Is Trigger:");
    if (ImGui::Button(c.isTrigger ? "Yes" : "No"))
        c.isTrigger = !c.isTrigger;
}