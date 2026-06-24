#include "LightPanel.h"
#include <glm/glm.hpp>
#include "scene/Light.h"

void LightPanel::Render(ECSWorld *ecs, entt::entity entity) {
    if (!ecs->HasComponent<LightComponent>(entity)) return;
    if (!ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen)) return;

    auto &light = ecs->GetComponent<LightComponent>(entity);

    RenderLightType(light);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    RenderLightSettings(light);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (light.type == LightType::DIRECTIONAL || light.type == LightType::SPOT)
        RenderLightDirection(light);

    RenderLightIntensity(light);
    RenderLightColors(light);

    if (light.type == LightType::POINT || light.type == LightType::SPOT)
        RenderLightAttenuation(light);

    if (light.type == LightType::SPOT)
        RenderSpotLightCone(light);
}

void LightPanel::RenderLightType(LightComponent &light) {
    const char *lightTypes[] = {"Directional", "Point", "Spot"};
    int currentType = static_cast<int>(light.type);

    ImGui::Text("Light Type");
    if (ImGui::Combo("##LightType", &currentType, lightTypes, 3)) {
        light.type = static_cast<LightType>(currentType);

        switch (light.type) {
            case LightType::DIRECTIONAL:
                light.ambient = glm::vec3(0.05f);
                light.diffuse = glm::vec3(0.4f);
                light.specular = glm::vec3(0.5f);
                break;
            case LightType::POINT:
                light.ambient = glm::vec3(0.05f);
                light.diffuse = glm::vec3(0.8f);
                light.specular = glm::vec3(1.0f);
                break;
            case LightType::SPOT:
                light.ambient = glm::vec3(0.0f);
                light.diffuse = glm::vec3(1.0f);
                light.specular = glm::vec3(1.0f);
                break;
            case LightType::NONE:
                light.ambient = glm::vec3(0.0f);
                light.diffuse = glm::vec3(0.0f);
                light.specular = glm::vec3(0.0f);
                break;
        }
    }
}

void LightPanel::RenderLightSettings(LightComponent &light) {
    ImGui::Checkbox("Active##Light", &light.isActive);
    ImGui::SameLine();
    ImGui::Checkbox("Cast Shadows", &light.castShadows);
}

void LightPanel::RenderLightDirection(LightComponent &light) {
    ImGui::Text("Direction");
    ImGui::DragFloat3("##Direction", &light.direction[0], 0.1f, -1.0f, 1.0f);

    if (glm::length(light.direction) > 0.001f)
        light.direction = glm::normalize(light.direction);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
}

void LightPanel::RenderLightIntensity(LightComponent &light) {
    ImGui::Text("Intensity");
    ImGui::DragFloat("##Intensity", &light.intensity, 0.01f, 0.0f, 10.0f);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
}

void LightPanel::RenderLightColors(LightComponent &light) {
    ImGui::Text("Ambient Color");
    float ambient[3] = {light.ambient.x, light.ambient.y, light.ambient.z};
    if (ImGui::ColorEdit3("##Ambient", ambient))
        light.ambient = glm::vec3(ambient[0], ambient[1], ambient[2]);

    ImGui::Spacing();

    ImGui::Text("Diffuse Color");
    float diffuse[3] = {light.diffuse.x, light.diffuse.y, light.diffuse.z};
    if (ImGui::ColorEdit3("##Diffuse", diffuse))
        light.diffuse = glm::vec3(diffuse[0], diffuse[1], diffuse[2]);

    ImGui::Spacing();

    ImGui::Text("Specular Color");
    float specular[3] = {light.specular.x, light.specular.y, light.specular.z};
    if (ImGui::ColorEdit3("##Specular", specular))
        light.specular = glm::vec3(specular[0], specular[1], specular[2]);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
}

void LightPanel::RenderLightAttenuation(LightComponent &light) {
    ImGui::Text("Attenuation");

    ImGui::Text("Constant");
    ImGui::DragFloat("##Constant", &light.constant, 0.01f, 0.0f, 10.0f);

    ImGui::Text("Linear");
    ImGui::DragFloat("##Linear", &light.linear, 0.001f, 0.0f, 1.0f);

    ImGui::Text("Quadratic");
    ImGui::DragFloat("##Quadratic", &light.quadratic, 0.001f, 0.0f, 1.0f);

    ImGui::Text("Radius");
    ImGui::DragFloat("##Radius", &light.radius, 0.5f, 1.0f, 100.0f);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
}

void LightPanel::RenderSpotLightCone(LightComponent &light) {
    ImGui::Text("Spot Cone");

    ImGui::Text("Inner Cutoff (degrees)");
    ImGui::DragFloat("##InnerCutoff", &light.innerCutoff, 0.5f, 0.0f, 90.0f);

    ImGui::Text("Outer Cutoff (degrees)");
    ImGui::DragFloat("##OuterCutoff", &light.outerCutoff, 0.5f, 0.0f, 90.0f);

    if (light.outerCutoff < light.innerCutoff)
        light.outerCutoff = light.innerCutoff;

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
}