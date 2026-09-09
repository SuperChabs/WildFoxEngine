#pragma once

#include <string>
#include <GLFW/glfw3.h>

/// @file Window.h
/// @brief Window class handler
/// @author SuperChabs
/// @date 2026-01-28

class Window {
    GLFWwindow *window;
    int width;
    int height;
    std::string title;

public:
    Window(int width, int height, std::string title);

    ~Window();

    Window(const Window &) = delete;

    Window &operator=(const Window &) = delete;

    Window(Window &&) = delete;

    Window &operator=(Window &&) = delete;

    bool Initialize();

    void Terminate();

    [[nodiscard]] bool ShouldClose() const;

    void SwapBuffers() const;

    static void PollEvents();

    GLFWwindow *GetGLFWWindow() const;
    [[nodiscard]] int GetWidth() const;
    [[nodiscard]] int GetHeight() const;
    [[nodiscard]] float GetAspectRatio() const;

    void SetFramebufferSizeCallback(GLFWframebuffersizefun callback) const;
    void SetCursorPosCallback(GLFWcursorposfun callback) const;
    void SetScrollCallback(GLFWscrollfun callback) const;
    void SetMouseButtonCallback(GLFWmousebuttonfun callback) const;
    void SetCursorMode(int mode) const;
    void SetSize(int w, int h);
};
