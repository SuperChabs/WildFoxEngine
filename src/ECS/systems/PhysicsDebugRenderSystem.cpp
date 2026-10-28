#include "PhysicsDebugRenderSystem.h"

#include <map>

#include <entt/entt.hpp>

namespace {
    struct Palette {
        glm::vec3 fill, edge, vertex;
    };

    constexpr Palette solidPalette   {{0.10f, 0.75f, 0.30f}, {0.30f, 1.00f, 0.45f}, {0.80f, 1.00f, 0.85f}};
    constexpr Palette triggerPalette {{0.10f, 0.70f, 0.60f}, {0.25f, 1.00f, 0.80f}, {0.80f, 1.00f, 0.95f}};

    constexpr float fillAlpha  = 0.18f;
    constexpr float edgeAlpha  = 0.90f;
    constexpr float pointPx    = 7.0f;
}

PhysicsDebugRenderSystem::PhysicsDebugRenderSystem() {
    SetupBuffers();
}

void PhysicsDebugRenderSystem::Update(ECSWorld &ecs, ShaderManager &shaderManager,
                                      const std::string &shaderName, const glm::mat4 &view,
                                      const glm::mat4 &projection) const {
    if (!m_enabled) return;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);
    glEnable(GL_PROGRAM_POINT_SIZE);

    shaderManager.Bind(shaderName);
    shaderManager.SetMat4(shaderName, "view", view);
    shaderManager.SetMat4(shaderName, "projection", projection);
    shaderManager.SetFloat(shaderName, "pointSize", pointPx);

    ecs.Each<TransformComponent, ColliderComponent>(
        [&](entt::entity, const TransformComponent &t, const ColliderComponent &c) {
            DebugMesh mesh;

            if (std::holds_alternative<Sphere>(c.shape))
                BuildSphereMesh(std::get<Sphere>(c.shape), t.position, mesh);
            else if (std::holds_alternative<ConvexHull>(c.shape))
                BuildHullMesh(std::get<ConvexHull>(c.shape), t.position, t.rotation, t.scale, mesh);
            else
                return;

            const auto &[fill, edge, vertex] = c.isTrigger ? triggerPalette : solidPalette;

            glDepthMask(GL_FALSE);
            shaderManager.SetInt(shaderName, "roundPoint", 0);
            shaderManager.SetVec3(shaderName, "color", fill);
            shaderManager.SetFloat(shaderName, "alpha", fillAlpha);
            Draw(GL_TRIANGLES, mesh.tris);
            glDepthMask(GL_TRUE);

            shaderManager.SetVec3(shaderName, "color", edge);
            shaderManager.SetFloat(shaderName, "alpha", edgeAlpha);
            Draw(GL_LINES, mesh.lines);

            shaderManager.SetInt(shaderName, "roundPoint", 1);
            shaderManager.SetVec3(shaderName, "color", vertex);
            shaderManager.SetFloat(shaderName, "alpha", 1.0f);
            Draw(GL_POINTS, mesh.points);
        });

    shaderManager.Unbind();
}

void PhysicsDebugRenderSystem::SetupBuffers() {
    VAO = std::make_unique<VertexArray>();
    VBO = std::make_unique<VertexBuffer>();

    VBO->Bind();
    VAO->Bind();

    VBO->SetData(nullptr, 144 * sizeof(glm::vec3), GL_DYNAMIC_DRAW);
    VAO->AddAttribute(0, 3, GL_FLOAT, false, 3 * sizeof(float), 0);

    VAO->Unbind();
}

void PhysicsDebugRenderSystem::Draw(const GLenum mode, const std::vector<glm::vec3> &pts) const {
    if (pts.empty()) return;

    VAO->Bind();
    VBO->Bind();

    VBO->SetData(pts.data(), pts.size() * sizeof(glm::vec3), GL_DYNAMIC_DRAW);
    glDrawArrays(mode, 0, static_cast<GLsizei>(pts.size()));

    VBO->Unbind();
    VAO->Unbind();
}

void PhysicsDebugRenderSystem::BuildSphereMesh(const Sphere &sphere, const glm::vec3 &pos, DebugMesh &mesh,
        const int segments, const int rings) {
    const float r = sphere.m_radius + 0.1f;

    const auto P = [&](const int ring, const int seg) {
        const float theta = glm::pi<float>() * static_cast<float>(ring) / static_cast<float>(rings);
        const float phi   = glm::two_pi<float>() * static_cast<float>(seg % segments) / static_cast<float>(segments);
        return pos + r * glm::vec3(std::sin(theta) * std::cos(phi), std::cos(theta), std::sin(theta) * std::sin(phi));
    };

    mesh.points.push_back(P(0, 0));
    mesh.points.push_back(P(rings, 0));
    for (int i = 1; i < rings; ++i)
        for (int j = 0; j < segments; ++j)
            mesh.points.push_back(P(i, j));

    for (int i = 0; i < rings; ++i) {
        for (int j = 0; j < segments; ++j) {
            const glm::vec3 a = P(i,     j);
            const glm::vec3 b = P(i,     j + 1);
            const glm::vec3 c = P(i + 1, j);
            const glm::vec3 d = P(i + 1, j + 1);

            if (i != 0) {
                mesh.tris.push_back(a);
                mesh.tris.push_back(c);
                mesh.tris.push_back(b);
            }
            if (i != rings - 1) {
                mesh.tris.push_back(b);
                mesh.tris.push_back(c);
                mesh.tris.push_back(d);
            }

            mesh.lines.push_back(a); mesh.lines.push_back(c);
            if (i != 0) { mesh.lines.push_back(a); mesh.lines.push_back(b); }
        }
    }
}

std::vector<glm::vec3> PhysicsDebugRenderSystem::BuildLines(const AABB &aabb, const glm::vec3 &pos) {
    glm::vec3 mn = aabb.min + pos;
    glm::vec3 mx = aabb.max + pos;

    glm::vec3 v[8] =
    {
        {mn.x, mn.y, mn.z},
        {mx.x, mn.y, mn.z},
        {mx.x, mx.y, mn.z},
        {mn.x, mx.y, mn.z},
        {mn.x, mn.y, mx.z},
        {mx.x, mn.y, mx.z},
        {mx.x, mx.y, mx.z},
        {mn.x, mx.y, mx.z}
    };

    std::vector<glm::vec3> pts;

    int edges[12][2] = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0},
        {4, 5}, {5, 6}, {6, 7}, {7, 4},
        {0, 4}, {1, 5}, {2, 6}, {3, 7}
    };

    for (auto &e: edges) {
        pts.push_back(v[e[0]]);
        pts.push_back(v[e[1]]);
    }

    return pts;
}

std::vector<glm::vec3> PhysicsDebugRenderSystem::BuildSphereLines(const Sphere &sphere, const glm::vec3 &pos, const int segments) {
    std::vector<glm::vec3> pts;
    const float r = sphere.m_radius + 0.05f;

    auto addCircle = [&](const int axis) {
        for (int i = 0; i < segments; ++i) {
            const float a0 = (static_cast<float>(i) / segments) * glm::two_pi<float>();
            const float a1 = (static_cast<float>(i + 1) / segments) * glm::two_pi<float>();

            glm::vec3 p0, p1;
            if (axis == 0) {
                p0 = { r * std::cos(a0), r * std::sin(a0), 0.0f };
                p1 = { r * std::cos(a1), r * std::sin(a1), 0.0f };
            }
            if (axis == 1) {
                p0 = { r * std::cos(a0), 0.0f, r * std::sin(a0) };
                p1 = { r * std::cos(a1), 0.0f, r * std::sin(a1) };
            }
            if (axis == 2) {
                p0 = { 0.0f, r * std::cos(a0), r * std::sin(a0) };
                p1 = { 0.0f, r * std::cos(a1), r * std::sin(a1) };
            }

            pts.push_back(p0 + pos);
            pts.push_back(p1 + pos);
        }
    };

    addCircle(0);
    addCircle(1);
    addCircle(2);

    return pts;
}

void PhysicsDebugRenderSystem::BuildHullMesh(const ConvexHull &hull, const glm::vec3 &pos, const glm::quat &rot,
    const glm::vec3 &scale, DebugMesh &mesh) {
    std::vector<glm::vec3> world(hull.m_points.size());
    for (size_t i = 0; i < world.size(); ++i)
        world[i] = pos + rot * (hull.m_points[i] * scale);

    mesh.points = world;

    struct EdgeInfo {
        glm::vec3 normal;
        bool paired = false;
        bool sharp  = false;
    };
    std::map<std::pair<int, int>, EdgeInfo> edges;

    for (const Tri &t : hull.m_tris) {
        const glm::vec3 &a = world[t.a], &b = world[t.b], &c = world[t.c];

        mesh.tris.push_back(a);
        mesh.tris.push_back(b);
        mesh.tris.push_back(c);

        glm::vec3 n = glm::cross(b - a, c - a);
        const float len = glm::length(n);
        n = len > 1e-8f ? n / len : glm::vec3(0, 1, 0);

        const int idx[3] = {t.a, t.b, t.c};
        for (int k = 0; k < 3; ++k) {
            const int i = idx[k], j = idx[(k + 1) % 3];
            const std::pair key{std::min(i, j), std::max(i, j)};

            auto [it, inserted] = edges.try_emplace(key, EdgeInfo{n});
            if (!inserted) {
                it->second.paired = true;
                it->second.sharp  = glm::dot(it->second.normal, n) < 0.9999f;
            }
        }
    }

    for (const auto &[key, e] : edges) {
        if (e.paired && !e.sharp) continue;
        mesh.lines.push_back(world[key.first]);
        mesh.lines.push_back(world[key.second]);
    }
}
