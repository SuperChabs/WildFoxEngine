#include "Engine.h"

#include "utils/Logger.h"

#include <imgui.h>

Engine::Engine(int w, int h, const std::string& title) 
    : Application(w, h, title)
{
}

void Engine::OnInitialize()
{
    //Logger::Log(LogLevel::INFO, "test", true);

    Logger::Log(LogLevel::INFO, "Initializing game...");
    
    // Завантаження шейдерів
    mainShader = std::make_unique<Shader>("basic");
    skyboxShader = std::make_unique<Shader>("skybox"); //
    
    // Завантаження текстур
    diffuseMap = GetTextureManager()->LoadTexture("assets/textures/bricks2.jpg");
    normalMap = GetTextureManager()->LoadTexture("assets/textures/bricks2_normal.jpg");
    depthMap = GetTextureManager()->LoadTexture("assets/textures/bricks2_disp.jpg");
    
    // Створення скайбоксу
    std::vector<std::string> faces = 
    {
        "assets/textures/skybox3/right.png",
        "assets/textures/skybox3/left.png",
        "assets/textures/skybox3/top.png",
        "assets/textures/skybox3/bottom.png",
        "assets/textures/skybox3/front.png",
        "assets/textures/skybox3/back.png"
    };
    
    unsigned int cubemapTexture = GetTextureManager()->LoadCubemap(faces);
    skybox = std::make_unique<Skybox>(cubemapTexture, skyboxShader.get());
    
    Logger::Log(LogLevel::INFO, "Creating EditorLayout...");
    editorLayout = std::make_unique<EditorLayout>();
    
    if (!editorLayout)
        Logger::Log(LogLevel::ERROR, "Failed to create EditorLayout!");
    else
        Logger::Log(LogLevel::INFO, "EditorLayout created successfully!");

    // Налаштування шейдерів
    mainShader->use();
    mainShader->setInt("diffuseMap", 0);
    mainShader->setInt("normalMap", 1);
    mainShader->setInt("depthMap", 2);
    
    skyboxShader->use();
    skyboxShader->setInt("skybox", 0);
    
    // Ініціалізація змінних
    lightPos = glm::vec3(0.5f, 1.0f, 0.3f);
    heightScale = 0.1f;
    
    Logger::Log(LogLevel::INFO, "Game initialized successfully");
}

void Engine::OnUpdate(float deltaTime)
{
    if (GetInput()->IsKeyPressed(XKey::KEY_Q))
    {
        if (heightScale > 0.0f)
            heightScale -= 0.5f * deltaTime;
        else
            heightScale = 0.0f;
    }
    else if (GetInput()->IsKeyPressed(XKey::KEY_E))
    {
        if (heightScale < 1.0f)
            heightScale += 0.5f * deltaTime;
        else
            heightScale = 1.0f;
    }

    if (GetSceneManager()->GetObjectCount() > 0 && gameObject)
    {
        for (const auto& [id, obj] : GetSceneManager()->GetObjects()) 
        {
            auto& params = objectParams[obj->objectID];
            
            if (params.autoRotate)
                obj->transform.Rotate(glm::vec3(0.0f, params.rotateSpeed * deltaTime, 0.0f));
            
        }
    }
}

void Engine::RenderUI()
{    
    if (!editorLayout)
    {
        Logger::Log(LogLevel::ERROR, "EditorLayout is null!");
        return;
    }

    // Просто рендеримо UI панелі
    editorLayout->RenderEditor(
        GetSceneManager(),
        GetCamera(),
        GetRenderer(),
        [this]() {
            Mesh* gameMesh = PrimitivesFactory::CreatePrimitive(PrimitiveType::CUBE);
            if (gameMesh)
            {
                std::unique_ptr<Model> gameModel = std::make_unique<Model>(gameMesh, "Cube");
                SceneObject* newObj = GetSceneManager()->AddObject("Cube", std::move(gameModel));
                objectParams[newObj->objectID] = ObjectParams();
                Logger::Log(LogLevel::INFO, "Cube created with ID: " + std::to_string(newObj->objectID));
            }
        }
    );
}

void Engine::OnRender()
{
    // === РЕНДЕР 3D СЦЕНИ В FRAMEBUFFER ===
    if (editorLayout && editorLayout->GetFramebuffer())
    {
        Framebuffer* fb = editorLayout->GetFramebuffer();
        ImVec2 viewportSize = editorLayout->GetViewportSize();
        
        // Прив'язуємо framebuffer
        fb->Bind();
        
        // Очищаємо (тестовий червоний колір)
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        
        if (viewportSize.x > 0 && viewportSize.y > 0)
        {
            // Матриці для 3D
            glm::mat4 projection = glm::perspective(
                glm::radians(GetCamera()->GetZoom()), 
                viewportSize.x / viewportSize.y, 
                0.1f, 100.0f
            );
            glm::mat4 view = GetCamera()->GetViewMatrix();
            
            // Рендеримо 3D сцену
            mainShader->use();
            mainShader->setMat4("projection", projection);
            mainShader->setMat4("view", view);
            mainShader->setVec3("viewPos", GetCamera()->GetPosition());
            mainShader->setVec3("lightPos", lightPos);
            mainShader->setFloat("height_scale", heightScale);
            
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, diffuseMap);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, normalMap);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, depthMap);
            
            GetSceneManager()->RenderAll(*mainShader);
            skybox->Render(*GetCamera(), projection);
        }
        
        // Відв'язуємо framebuffer
        fb->Unbind();
    }
    
    // === ПІДГОТОВКА ЕКРАНУ ДЛЯ IMGUI ===
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, GetWindow()->GetWidth(), GetWindow()->GetHeight());
    glClearColor(0.15f, 0.15f, 0.15f, 1.0f);  // Темно-сірий фон
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
}

// void Engine::Menu()
// {
//     ImGui::Begin("Menu", nullptr, ImGuiWindowFlags_MenuBar);
//
//     if (ImGui::BeginMenuBar())
//     {
//         if (ImGui::BeginMenu("File"))
//         {
//             if (ImGui::MenuItem("Open..", "Ctrl+O")) { Logger::Log(LogLevel::INFO, "There is no funktional"); }
//             if (ImGui::MenuItem("Save", "Ctrl+S"))   { Logger::Log(LogLevel::INFO, "There is no funktional"); }
//             if (ImGui::MenuItem("Close", "Ctrl+W"))  { Stop(); }
//             ImGui::EndMenu();
//         }
//        
//         if (ImGui::BeginMenu("Create")) 
//         {
//             if (ImGui::MenuItem("Cube")) 
//             {
//                 Mesh* gameMesh = PrimitivesFactory::CreatePrimitive(PrimitiveType::CUBE);
//                 if (gameMesh)
//                 {
//                     std::unique_ptr<Model> gameModel = std::make_unique<Model>(gameMesh, "Cube");
//                     gameObject = GetSceneManager()->AddObject("Cube", std::move(gameModel));
//
//                     objectParams[gameObject->objectID] = ObjectParams();
//
//                     Logger::Log(LogLevel::INFO, "Cube created and added to scene with ID: " + std::to_string(gameObject->objectID));
//                 }
//             }
//            
//             ImGui::EndMenu();
//         }
//         ImGui::EndMenuBar();
//     }
//
//     float fps = 1.0f / GetTime()->GetDeltaTime();
//     ImGui::Text("FPS: %.0f", fps);
//     ImGui::Text("Frame time: %.3f ms", GetTime()->GetDeltaTime() * 1000.0f);
//   
//     ImGui::Separator();
//
//     if (ImGui::CollapsingHeader("Camera")) 
//     {
//         glm::vec3 pos = GetCamera()->GetPosition();
//         ImGui::Text("Position: (%.1f, %.1f, %.1f)", pos.x, pos.y, pos.z);
//        
//         float speed = GetCamera()->GetMovementSpeed();
//         if (ImGui::SliderFloat("Speed", &speed, 1.0f, 20.0f)) 
//             GetCamera()->SetMovementSpeed(speed);
//
//         float sensitivity = GetCamera()->GetMouseSensitivity();
//         if (ImGui::SliderFloat("Sensitivity", &sensitivity, 0.01f, 1.0f)) 
//             GetCamera()->SetMouseSensitivity(sensitivity);
//
//         if (ImGui::SliderFloat3("Position", &pos[0], -10.0f, 10.0f)) 
//             GetCamera()->SetPosition(pos);
//
//         float yaw = GetCamera()->GetYaw();
//         if(ImGui::SliderFloat("Yaw", &yaw, -360.0f, 360.0f)) 
//         {
//             GetCamera()->SetYaw(yaw);
//         }
//
//         float pitch = GetCamera()->GetPitch();
//         if(ImGui::SliderFloat("Pitch", &pitch, -90.0f, 90.0f)) 
//         {
//             GetCamera()->SetPitch(pitch);
//         }
//
//         ImGui::Separator();
//
//         if (ImGui::Button("Reset Position")) 
//             GetCamera()->SetPosition(glm::vec3(0.0f, 0.0f, 3.0f));
//
//         if (ImGui::Button("Reset Orientation")) 
//         {
//             GetCamera()->SetYaw(-95.0f);
//             GetCamera()->SetPitch(0.0f);
//         }
//
//         if (ImGui::Button("Reset All")) 
//         {
//             GetCamera()->SetPosition(glm::vec3(0.0f, 0.0f, 3.0f));
//             GetCamera()->SetYaw(-95.0f);
//             GetCamera()->SetPitch(0.0f);
//         }
//
//         if (ImGui::Button(cameraControlEnabled ? "Disable Camera Control" : "Enable Camera Control")) 
//             SetCameraControlMode(!cameraControlEnabled);
//     }
//
//     if (ImGui::CollapsingHeader("Window")) 
//     {
//         ImGui::Text("Size: %dx%d", GetWindow()->GetWidth(), GetWindow()->GetHeight());
//         ImGui::Text("Aspect: %.2f", GetWindow()->GetAspectRatio());
//     }
//
//     ImGui::Separator();
//     ImGui::Text("Press F1 to toggle this window");
//
//     ImGui::End();
// }
//
// void Engine::SceneSettings()
//     {
//         ImGui::Begin("Scene Settings");
//
//         ImGui::Text("Light Position");
//         if (ImGui::SliderFloat3("Light Pos", &lightPos[0], -5.0f, 5.0f))
//             mainShader->setVec3("lightPos", lightPos);
//
//         ImGui::Text("Height Scale");
//         if (ImGui::SliderFloat("Height Scale", &heightScale, 0.0f, 1.0f))
//             mainShader->setFloat("height_scale", heightScale);
//
//         ImGui::Separator();
//
//         if (GetSceneManager()->GetObjectCount() > 0) 
//         {
//             uint64_t idToDelete = 0;
//
//             for (const auto& [id, obj] : GetSceneManager()->GetObjects()) 
//             {
//                 std::string headerLabel = obj->name + " (ID: " + std::to_string(obj->objectID) + " )"; 
//   
//                 ImGui::PushID(static_cast<int>(obj->objectID));
//
//                 if (ImGui::CollapsingHeader(headerLabel.c_str())) 
//                 {
//                     glm::vec3 pos = obj->transform.GetPosition();
//                     if (ImGui::SliderFloat3("Position", &pos[0], -10.0f, 10.0f))
//                         obj->transform.SetPosition(pos);
//
//                     glm::vec3 rot = obj->transform.GetRotation();
//                     if (ImGui::SliderFloat3("Rotation", &rot[0], -360.0f, 360.0f))
//                         obj->transform.SetRotation(rot);
//
//                     float scale = obj->transform.GetScale().x; 
//                     if (ImGui::SliderFloat("Scale", &scale, 0.01f, 5.0f))
//                         obj->transform.SetScale(scale); 
//
//                     ImGui::Separator();
//
//                     auto& params = objectParams[obj->objectID]; 
//
//                     ImGui::Checkbox("Auto Rotate", &params.autoRotate);
//                     ImGui::DragFloat("Rotate Speed", &params.rotateSpeed, 1.0f, -1000.0f, 1000.0f);
//
//                     ImGui::Separator();
//
//                     if (ImGui::CollapsingHeader("Material"))
//                     {
//                         if (ImGui::Checkbox("Use Solid Color", &obj->params.useColor))
//                             if (obj->params.useColor)
//                                 obj->model->SetColor(obj->params.color);  
//
//                         if (obj->params.useColor)
//                             if (ImGui::ColorEdit3("Color", &obj->params.color[0]))
//                                 obj->model->SetColor(obj->params.color);
//                         else
//                         {
//                             ImGui::Text("Texture: %s", obj->params.texturePath.empty() ? "None" : obj->params.texturePath.c_str());
//
//                             if (ImGui::Button("Load Texture"))
//                             {
//                                 std::string texPath = "assets/textures/bricks2.jpg";
//                                 obj->params.texturePath = texPath;
//
//                                 unsigned int texID = GetTextureManager()->LoadTexture(texPath);
//                                 Texture tex;
//                                 tex.id = texID;
//                                 tex.type = "texture_diffuse";
//                                 tex.path = texPath;
//
//                                 std::vector<Texture> textures = {tex};
//                                 obj->model->SetTextures(textures);  
//                             }
//                         }
//                     }
//
//                     ImGui::Separator();
//
//                     if (ImGui::Button("Delete Object"))
//                     {
//                         idToDelete = obj->objectID;
//
//                         ImGui::PopID();
//
//                         break;
//                     }
//                 }
//
//                 ImGui::PopID();
//             }
//
//             if (idToDelete != 0) 
//             {
//                 GetSceneManager()->RemoveObject(idToDelete);
//                 objectParams.erase(idToDelete);
//                 Logger::Log(LogLevel::INFO, "Object " + std::to_string(idToDelete) + " deleted");
//             }
//         }
//         else
//         {
//             ImGui::Text("No objects in scene");
//             ImGui::Text("Use Create -> Cube to add objects");
//         }
//
//         ImGui::End();
//     }


void Engine::OnShutdown()
{
    Logger::Log(LogLevel::INFO, "Shutting down game...");
    
    mainShader.reset();
    skyboxShader.reset();
    skybox.reset();
    
    Logger::Log(LogLevel::INFO, "Game shutdown complete!");
}