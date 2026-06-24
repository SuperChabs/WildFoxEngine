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
    void RenderLightType(LightComponent &light);

    void RenderLightSettings(LightComponent &light);

    void RenderLightDirection(LightComponent &light);

    void RenderLightIntensity(LightComponent &light);

    void RenderLightColors(LightComponent &light);

    void RenderLightAttenuation(LightComponent &light);

    void RenderSpotLightCone(LightComponent &light);
};