#include "EditorPicking.h"

#include <limits>

#include "ECS/components/Components.h"
#include "physics/queries/RayCast.h"
#include "scene/Mesh.h"

Ray EditorPicking::ScreenToRay(const ImVec2 mouse, const ImVec2 viewportPos, const ImVec2 viewportSize,
                               const glm::mat4 &view, const glm::mat4 &proj) {
    const float x = 2.0f * (mouse.x - viewportPos.x) / viewportSize.x - 1.0f;
    const float y = 1.0f - 2.0f * (mouse.y - viewportPos.y) / viewportSize.y;

    const glm::mat4 invViewProj = glm::inverse(proj * view);
    glm::vec4 nearPt = invViewProj * glm::vec4(x, y, -1.0f, 1.0f);
    glm::vec4 farPt  = invViewProj * glm::vec4(x, y,  1.0f, 1.0f);
    nearPt /= nearPt.w;
    farPt  /= farPt.w;

    return {glm::vec3(nearPt), glm::normalize(glm::vec3(farPt - nearPt))};
}

static bool MeshBounds(const Mesh &mesh, glm::vec3 &mins, glm::vec3 &maxs) {
    const auto &vertices = mesh.GetVertices();
    if (vertices.empty()) return false;

    mins = glm::vec3(std::numeric_limits<float>::max());
    maxs = glm::vec3(std::numeric_limits<float>::lowest());
    for (const Vertex &v : vertices) {
        mins = glm::min(mins, v.Position);
        maxs = glm::max(maxs, v.Position);
    }
    return true;
}

RayCastHit EditorPicking::Pick(ECSWorld &world, const Ray &ray, const float maxDist) {
    RayCastHit best;

    auto consider = [&](const entt::entity e, const HitInfo &h) {
        if (h.hit && h.distance < best.hitInfo.distance) {
            best.hitInfo = h;
            best.entity = e;
        }
    };

    world.Each<c_Transform>([&](const entt::entity e, const c_Transform &) {
        if (world.HasComponent<c_Visibility>(e)) {
            const auto &vis = world.GetComponent<c_Visibility>(e);
            if (!vis.visible || !vis.isActive) return;
        }

        const glm::mat4 model = world.GetGlobalTransform(e);

        if (world.HasComponent<c_Collider>(e)) {
            consider(e, RayCast::Collider(ray, world.GetComponent<c_Collider>(e), model, maxDist));
        } else if (world.HasComponent<c_Mesh>(e)) {
            const auto &mesh = world.GetComponent<c_Mesh>(e).mesh;
            glm::vec3 mins, maxs;
            if (mesh && MeshBounds(*mesh, mins, maxs))
                consider(e, RayCast::LocalAABB(ray, model, mins, maxs, maxDist));
        }

        // icons are camera-facing quads, a sphere around the position is close enough
        if (world.HasComponent<c_Icon>(e)) {
            const float radius = world.GetComponent<c_Icon>(e).scale * 0.5f;
            consider(e, RayCast::Sphere(ray, glm::vec3(model[3]), radius, maxDist));
        }
    });

    return best;
}

entt::entity EditorPicking::ResolveSelectable(ECSWorld &world, entt::entity entity) {
    entt::entity e = entity;
    for (int depth = 0; depth < 64 && e != entt::null && world.IsValid(e); ++depth, e = world.GetParent(e)) {
        if (world.HasComponent<c_Model>(e))
            return e;
    }
    return entity;
}
