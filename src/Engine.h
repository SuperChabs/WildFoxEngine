
#ifndef ENGINE_H
#define ENGINE_H

#include "application/Application.h"
#include "core/Shader.h"
#include "rendering/Skybox.h"
#include "rendering/primitive/PrimitivesFactory.h"
#include "scene/SceneManager.h"
#include "UI/EditorLayout.h"

#include <glm/glm.hpp>

#include <memory>
#include <unordered_map>
#include <string>

class Engine : public Application 
{
private:
    std::unique_ptr<Shader> mainShader;
    std::unique_ptr<Shader> skyboxShader;
    std::unique_ptr<Skybox> skybox;

    std::unique_ptr<EditorLayout> editorLayout;
    
    unsigned int diffuseMap;
    unsigned int normalMap;
    unsigned int depthMap;
    
    glm::vec3 lightPos;
    float heightScale;

    SceneObject* gameObject = nullptr;
    bool gameAutoRotate = false;
    float gameRotateSpeed = 50.0f; 

    std::unordered_map<uint64_t, ObjectParams> objectParams;

protected:
    void OnInitialize() override;
    void OnUpdate(float deltaTime) override;
    void OnRender() override;
    void OnShutdown() override;

    void RenderUI() override;
    // void Menu();
    // void SceneSettings();

public:
    Engine(int w, int h, const std::string& title);
};

#endif