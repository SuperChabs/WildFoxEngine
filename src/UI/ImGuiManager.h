#pragma once

#include <GLFW/glfw3.h>

class ImGuiManager {
    bool initialized;

public:
    ImGuiManager();
    ~ImGuiManager();

    ImGuiManager(const ImGuiManager &) = delete;

    ImGuiManager &operator=(const ImGuiManager &) = delete;

    static ImGuiManager &Instance();

    bool Initialize(GLFWwindow *window);
    void Shutdown();

    void BeginFrame(bool cameraControl);
    void EndFrame();

    void RedFoxTheme();

    [[nodiscard]]
    bool IsInitialized() const;
};