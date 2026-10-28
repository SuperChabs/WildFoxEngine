#include "PrefabsCommands.h"

#include "core/CommandManager.h"
#include "core/logging/Logger.h"

#include "ECS/components/Components.h"
#include "physics/shapeFunc/ShapeFunctions.h"

void PrefabsCommands::RegisterPrefabsCommands(ECSModule *ecsModule, ResourceModule *resModule) {
    CommandManager::RegisterCommand("PrFb_CreateSandbox",
        [ecsModule, resModule](const CommandArgs &) {
            ECSWorld *world = ecsModule->GetECS();

            auto createStaticBox = [&](const glm::vec3 &position, const glm::vec3 &scale,
                                       const float elasticity, const float friction) {
                const auto entity = resModule->GetModelManager()->LoadWithECS(
                    "assets/objects/shapes/cube/cube.obj",
                    world,
                    true
                );
                if (entity == entt::null) return;

                const glm::vec3 h = scale * 0.5f;
                std::vector<glm::vec3> points = {
                    {-h.x, -h.y, -h.z}, { h.x, -h.y, -h.z}, { h.x,  h.y, -h.z}, {-h.x,  h.y, -h.z},
                    {-h.x, -h.y,  h.z}, { h.x, -h.y,  h.z}, { h.x,  h.y,  h.z}, {-h.x,  h.y,  h.z}
                };

                Box shape;
                Shape::Build(shape, points, static_cast<int>(points.size()));

                world->AddComponent<TransformComponent>(
                    entity, position, glm::identity<glm::quat>(), scale
                );

                world->AddComponent<ColliderComponent>(entity, ColliderComponent{
                    .shape = shape,
                    .isTrigger = false
                });

                world->AddComponent<RigidBodyComponent>(entity, RigidBodyComponent{
                    .m_invMass = 0.0f,
                    .m_elasticity = elasticity,
                    .m_friction = friction,
                    .m_linearVelocity = glm::vec3(0.0f),
                    .m_angularVelocity = glm::vec3(0.0f),
                    .m_forceAccum = glm::vec3(0.0f),
                    .m_torqueAccum = glm::vec3(0.0f)
                });
            };

            createStaticBox({0.0f, -2.0f, 0.0f}, {10.0f, 0.5f, 10.0f}, 0.2f, 0.8f);

            constexpr float wallH = 4.0f;
            constexpr float wallY = 0.25f;
            constexpr float wallT = 0.5f;
            constexpr float offset = 5.0f + wallT * 0.5f;

            createStaticBox({ offset, wallY, 0.0f}, {wallT, wallH, 10.0f}, 0.2f, 0.8f);
            createStaticBox({-offset, wallY, 0.0f}, {wallT, wallH, 10.0f}, 0.2f, 0.8f);

            createStaticBox({0.0f, wallY,  offset}, {10.0f + wallT * 2.0f, wallH, wallT}, 0.2f, 0.8f);
            createStaticBox({0.0f, wallY, -offset}, {10.0f + wallT * 2.0f, wallH, wallT}, 0.2f, 0.8f);

            Logger::Log(LogLevel::INFO, "Physics sandbox created");
        }
    );
}
