#pragma once

#include <unordered_map>
#include <vector>

#include <glm/glm.hpp>

#include "glad/glad.h"

struct ShadowOutputs {
    std::vector<glm::mat4> *lightMatrices = nullptr;
    std::unordered_map<entt::entity, int> *dirSpotIndices = nullptr;
    std::unordered_map<entt::entity, int> *cubeIndices    = nullptr;

    GLuint dirSpotArray = 0;
    GLuint cubeArray = 0;

    float pointFarPlane = 100.0f;
};

struct FrameContext {
    ShadowOutputs shadows;

    glm::mat4 view;
    glm::mat4 projection;

    glm::vec3 cameraPos;

    GLuint targetFBO = 0;
    GLuint sceneFBO = 0;
    GLuint sceneColor = 0;

    int width = 0;
    int height = 0;
};
