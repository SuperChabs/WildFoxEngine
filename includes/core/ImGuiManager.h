#ifndef IMGUI_MANAGER_H
#define IMGUI_MANAGER_H

#include <GLFW/glfw3.h>

class ImGuiManager 
{
private:
    bool initialized;
    
public:
    ImGuiManager();
    ~ImGuiManager();
    
    bool Initialize(GLFWwindow* window);
    void Shutdown();
    
    void BeginFrame();
    void EndFrame();
    
    bool IsInitialized() const { return initialized; }
};

#endif