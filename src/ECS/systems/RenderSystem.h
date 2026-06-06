#pragma once

#include <string>
#include <memory>

#include <entt/entt.hpp>
#include <glm/glm.hpp>

#include "ECS/components/Components.h"
#include "ECS/World.h"
#include "core/logging/Logger.h"
#include "resource/shader/ShaderManager.h"

class RenderSystem {
public:
    static void Update(ECSWorld &world, ShaderManager &shaderManager, const std::string &name) {
        shaderManager.Bind(name);

        world.Each<TransformComponent, MeshComponent, MaterialComponent, VisibilityComponent>(
            [&](entt::entity entity,
                TransformComponent &transform,
                MeshComponent &meshComp,
                MaterialComponent &matComp,
                VisibilityComponent &vis) {
                if (!vis.isActive || !vis.visible) return;

                const glm::mat4 modelMatrix = world.GetGlobalTransform(entity);
                shaderManager.SetMat4(name, "model", modelMatrix);

                if (matComp.material) {
                    if (meshComp.mesh) {
                        shaderManager.SetVec2(name, "tiling", world.GetComponent<MaterialComponent>(entity).tiling);
                        meshComp.mesh->Draw(shaderManager, name, matComp.material);
                    }
                } else if (world.HasComponent<ColorComponent>(entity)) {
                    const auto &color = world.GetComponent<ColorComponent>(entity);
                    if (meshComp.mesh)
                        meshComp.mesh->SetColor(color.color);

                    if (meshComp.mesh)
                        meshComp.mesh->Draw(shaderManager, name);
                } else {
                    if (meshComp.mesh)
                        meshComp.mesh->Draw(shaderManager, name);
                }
            });

        shaderManager.Unbind();
    }
};