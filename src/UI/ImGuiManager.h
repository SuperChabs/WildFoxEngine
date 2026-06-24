#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "UI/Theme.h"

class ImGuiManager {
private:
    ImGuiManager(const ImGuiManager &) = delete;

    ImGuiManager &operator=(const ImGuiManager &) = delete;

    bool initialized;
    ThemeStyle theme = ThemeStyle::Dark;

public:
    ImGuiManager();

    ~ImGuiManager();

    static ImGuiManager &Instance();

    bool Initialize(GLFWwindow *window);

    void Shutdown();

    void BeginFrame();

    void EndFrame();

    bool IsInitialized() const;

    void SetTheme(ThemeStyle t);

    ThemeStyle GetTheme() const;
};