#include "RenderSystem.h"
#include <entt/entt.hpp>
#include <glm/glm.hpp>

#include "ECS/components/Components.h"

void RenderSystem::Update(ECSWorld &world, ShaderManager &shaderManager, const std::string &name) {
    shaderManager.Bind(name);

    world.Each<c_Mesh, c_Material, c_Visibility>(
        [&](const entt::entity entity,
            const c_Mesh &meshComp,
            const c_Material &matComp,
            const c_Visibility &vis) {
            if (!vis.isActive || !vis.visible) return;

            const glm::mat4 modelMatrix = world.GetGlobalTransform(entity);
            shaderManager.SetMat4(name, "model", modelMatrix);

            if (matComp.material) {
                if (meshComp.mesh) {
                    shaderManager.SetVec2(name, "tiling", world.GetComponent<c_Material>(entity).tiling);
                    meshComp.mesh->Draw(shaderManager, name, matComp.material);
                }
            } else {
                if (meshComp.mesh)
                    meshComp.mesh->Draw(shaderManager, name);
            }
        });

    shaderManager.Unbind();
}