#pragma once

#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

/// @file Window.cppm
/// @brief Window class handler
/// @author SuperChabs
/// @date 2026-01-28

class Window {
private:
    GLFWwindow *window;
    int width;
    int height;
    std::string title;

public:
    Window(int width, int height, const std::string &title);

    ~Window();

    Window(const Window &) = delete;

    Window &operator=(const Window &) = delete;

    Window(Window &&) = delete;

    Window &operator=(Window &&) = delete;

    bool Initialize();

    void Terminate();

    bool ShouldClose() const;

    void SwapBuffers();

    void PollEvents();

    void SetFramebufferSizeCallback(GLFWframebuffersizefun callback);

    void SetCursorPosCallback(GLFWcursorposfun callback);

    void SetScrollCallback(GLFWscrollfun callback);

    void SetMouseButtonCallback(GLFWmousebuttonfun callback);

    void SetCursorMode(int mode);

    void SetSize(int w, int h);

    GLFWwindow *GetGLFWWindow();

    int GetWidth() const;

    int GetHeight() const;

    float GetAspectRatio() const;
};