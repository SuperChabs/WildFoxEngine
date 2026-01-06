module;

#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

export module XEngine.Core.ImGuiManager;

import XEngine.Core.Logger;

export class ImGuiManager 
{
private:
    bool initialized;
    
public:
    ImGuiManager() : initialized(false) {}
    ~ImGuiManager() { Shutdown(); }
    
    bool Initialize(GLFWwindow* window)
    {
        if (initialized) 
            return true;
        
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        
        ImGui::StyleColorsDark();
        
        const char* glsl_version = "#version 330"; 
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init(glsl_version);
        
        initialized = true;
        Logger::Log(LogLevel::INFO, "ImGui initialized");
        
        return true;
    }

    void Shutdown()
    {
        if (!initialized) 
            return;
        
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        
        initialized = false;
        Logger::Log(LogLevel::INFO, "ImGui shutdown completed");
    }
    
    void BeginFrame()
    {
        if (!initialized) 
            return;
        
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void EndFrame()
    {
        if (!initialized) 
            return;
        
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
    
    bool IsInitialized() const { return initialized; }
};