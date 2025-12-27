#include <imgui.h>
#include <glad/glad.h>

#include "application/Application.h"
#include "utils/Logger.h"

// крч хз, ну тіпа не клас
void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

Application::Application(int width, int height, const std::string& title)
    : window(nullptr), input(nullptr), time(nullptr), camera(nullptr),
      renderer(nullptr), textureManager(nullptr), sceneManager(nullptr),
      isRunning(false), showDebugUI(true), cameraControlEnabled(true)
{
    window = std::make_unique<Window>(width, height, title);

    Logger::Info("Application created");
}

Application::~Application()
{
    Shutdown();
}

bool Application::Initialize()
{
    if (!window->Initialize())
    {
        Logger::Error("Failed to initialize Window");
        return false;
    }

    glfwSetWindowUserPointer(window->GetGLFWWindow(), this);
    
    window->SetFramebufferSizeCallback(FramebufferSizeCallback);
    window->SetCursorPosCallback(MouseCallback);
    window->SetScrollCallback(Input::ScrollCallback);
    window->SetMouseButtonCallback(MouseButtonCallback);
    
    input = std::make_unique<Input>(window->GetGLFWWindow());
    time = std::make_unique<Time>();
    camera = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 3.0f));
    renderer = std::make_unique<Renderer>();
    textureManager = std::make_unique<TextureManager>();
    sceneManager = std::make_unique<SceneManager>();
    
    renderer->Initialize();
    
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

    imGuiManager = std::make_unique<ImGuiManager>();
    if (!imGuiManager->Initialize(window->GetGLFWWindow())) 
        return false;

    SetCameraControlMode(true);    
    
    Logger::Info("Application initialized successfully");
    
    OnInitialize();
    
    return true;
}

void Application::Run()
{
    isRunning = true;
    
    while (isRunning && !window->ShouldClose())
    {
        time->Update();
        
        ProcessInput();
        Update();
        Render();
        
        window->SwapBuffers();
        window->PollEvents();
    }
}

void Application::ProcessInput()
{
    if (input->IsKeyPressed(GLFW_KEY_ESCAPE)) 
        Stop();
    
    if (input->IsKeyJustPressed(GLFW_KEY_F1)) 
        showDebugUI = !showDebugUI;
    
    if (cameraControlEnabled) 
    {
        if (input->IsKeyPressed(GLFW_KEY_W)) 
            camera->ProcessKeyboard(FORWARD, time->GetDeltaTime());
        if (input->IsKeyPressed(GLFW_KEY_S)) 
            camera->ProcessKeyboard(BACKWARD, time->GetDeltaTime()); 
        if (input->IsKeyPressed(GLFW_KEY_A)) 
            camera->ProcessKeyboard(LEFT, time->GetDeltaTime());
        if (input->IsKeyPressed(GLFW_KEY_D)) 
            camera->ProcessKeyboard(RIGHT, time->GetDeltaTime());
        if (input->IsKeyPressed(GLFW_KEY_SPACE)) 
            camera->ProcessKeyboard(UP, time->GetDeltaTime());
        if (input->IsKeyPressed(GLFW_KEY_LEFT_SHIFT)) 
            camera->ProcessKeyboard(DOWN, time->GetDeltaTime());
    }
}

void Application::Update()
{
    float deltaTime = time->GetDeltaTime();
    sceneManager->Update(deltaTime);
    
    OnUpdate(deltaTime);
}

void Application::Render()
{
    renderer->BeginFrame();
    
    OnRender();

    if (showDebugUI) 
    {
        imGuiManager->BeginFrame();
        RenderDebugUI();
        imGuiManager->EndFrame();
    }    
    
    renderer->EndFrame();
}

void Application::RenderDebugUI() 
{
    ImGui::Begin("Debug Info");

    float fps = 1.0f / time->GetDeltaTime();
    ImGui::Text("FPS: %.1f", fps);
    ImGui::Text("Frame time: %.3f ms", time->GetDeltaTime() * 1000.0f);
    
    ImGui::Separator();

    if (ImGui::CollapsingHeader("Camera")) 
    {
        glm::vec3 pos = camera->GetPosition();
        ImGui::Text("Position: (%.1f, %.1f, %.1f)", pos.x, pos.y, pos.z);
        
        float speed = camera->GetMovementSpeed();
        if (ImGui::SliderFloat("Speed", &speed, 1.0f, 20.0f)) 
            camera->SetMovementSpeed(speed);

        float sensitivity = camera->GetMouseSensitivity();
        if (ImGui::SliderFloat("Sensitivity", &sensitivity, 0.01f, 1.0f)) 
            camera->SetMouseSensitivity(sensitivity);
    }

    if (ImGui::CollapsingHeader("Window")) 
    {
        ImGui::Text("Size: %dx%d", window->GetWidth(), window->GetHeight());
        ImGui::Text("Aspect: %.2f", window->GetAspectRatio());
    }
    
    ImGui::Separator();
    ImGui::Text("Press F1 to toggle this window");
    
    ImGui::End();
}

void Application::Shutdown()
{
    OnShutdown();
    
    Logger::Info("Application shutdown complete");
}

void Application::SetCameraControlMode(bool enabled) 
{
    cameraControlEnabled = enabled;
    
    if (enabled) 
    {
        window->SetCursorMode(GLFW_CURSOR_DISABLED);
        Logger::Info("Camera control: ON");
    } else 
    {
        window->SetCursorMode(GLFW_CURSOR_NORMAL);
        Logger::Info("Camera control: OFF (UI mode)");
    }
}



void Application::MouseCallback(GLFWwindow* window, double xpos, double ypos) 
{
    Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    
    if (!app || !app->input || !app->camera)
        return;
    
    app->input->UpdateMousePosition(xpos, ypos);
    
    if (app->cameraControlEnabled) 
    {
        glm::vec2 delta = app->input->GetMouseDelta();
        app->camera->ProcessMouseMovement(delta.x, delta.y);
    }
}

void Application::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) 
{
    Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    
    if (!app) return;
    
    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) 
        app->SetCameraControlMode(!app->cameraControlEnabled);
}