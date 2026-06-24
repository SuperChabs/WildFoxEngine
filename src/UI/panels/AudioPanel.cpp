#include "AudioPanel.h"
#include <string>
#include <cstdio>

void AudioPanel::Render(ECSWorld *ecs, entt::entity entity) {
    bool hasSource = ecs->HasComponent<AudioSourceComponent>(entity);
    bool hasListener = ecs->HasComponent<AudioListenerComponent>(entity);
    if (!hasSource && !hasListener) return;

    if (!ImGui::CollapsingHeader("Audio"))
        return;

    if (hasSource)
        RenderSource(ecs, entity);

    if (hasListener)
        RenderListener(ecs, entity);
}

void AudioPanel::RenderSource(ECSWorld *ecs, entt::entity entity) {
    auto &audio = ecs->GetComponent<AudioSourceComponent>(entity);

    ImGui::Text("Audio Source");
    ImGui::Separator();

    ImGui::Text("Current Path: %s", audio.path.empty() ? "<none>" : audio.path.c_str());
    ImGui::InputText("New Path", m_pathBuffer, sizeof(m_pathBuffer));

    if (ImGui::Button("Apply Audio Path", ImVec2(-1, 0))) {
        if (m_pathBuffer[0] != '\0') {
            audio.path = std::string(m_pathBuffer);
            audio._playing = false;
            m_pathBuffer[0] = '\0';
        }
    }

    ImGui::DragFloat("Volume", &audio.volume, 0.01f, 0.0f, 10.0f);
    ImGui::DragFloat("Pitch", &audio.pitch, 0.01f, 0.1f, 4.0f);
    ImGui::Checkbox("Loop", &audio.loop);
    ImGui::Checkbox("Spatial", &audio.spatical);
    ImGui::Checkbox("Play on Start", &audio.playOnStart);

    ImGui::Spacing();
}

void AudioPanel::RenderListener(ECSWorld *ecs, entt::entity entity) {
    ImGui::Text("Audio Listener");
    ImGui::Separator();
    ImGui::TextWrapped("This entity is used as the OpenAL listener if present.");
    ImGui::Spacing();
}