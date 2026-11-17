#pragma once

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <ImGuizmo.h>
#include <glm/gtc/matrix_transform.hpp>

#include "ECS/World.h"
#include "ECS/components/Components.h"

class LightPanel {
public:
    void Render(ECSWorld *ecs, entt::entity entity);

private:
    void RenderLightType(c_Light &light);

    void RenderLightSettings(c_Light &light);

    void RenderLightDirection(c_Light &light);

    void RenderLightIntensity(c_Light &light);

    void RenderLightColors(c_Light &light);

    void RenderLightAttenuation(c_Light &light);

    void RenderSpotLightCone(c_Light &light);
};