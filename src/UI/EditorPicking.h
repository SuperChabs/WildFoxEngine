#pragma once

#include <imgui.h>
#include <glm/glm.hpp>

#include "ECS/World.h"
#include "physics/queries/RayStruct.h"

namespace EditorPicking {
    // ray from the camera through a screen point inside the viewport rectangle
    Ray ScreenToRay(ImVec2 mouse, ImVec2 viewportPos, ImVec2 viewportSize, const glm::mat4 &view, const glm::mat4 &proj);

    // closest visible entity hit by the ray: collider if present, else mesh bounds; icons are always tested
    RayCastHit Pick(ECSWorld &world, const Ray &ray, float maxDist);

    // entities that belong to an imported model resolve to the model root
    entt::entity ResolveSelectable(ECSWorld &world, entt::entity entity);
}
