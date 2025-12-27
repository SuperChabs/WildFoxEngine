#ifndef INPUT_H
#define INPUT_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <memory>
#include <unordered_map>

class Input 
{
private:
    GLFWwindow* window;
    
    float lastMouseX;
    float lastMouseY;
    bool firstMouse;
    
    float mouseDeltaX;
    float mouseDeltaY;

    mutable std::unordered_map<int, bool> keyStates;

public:
    static float scrollOffsetX;
    static float scrollOffsetY;
    
    Input(GLFWwindow* window);
    
    bool IsKeyPressed(int key) const;
    bool IsKeyReleased(int key) const;
    bool IsKeyDown(int key) const;
    bool IsKeyJustPressed(int key) const;
    
    void UpdateMousePosition(double xpos, double ypos);
    glm::vec2 GetMouseDelta();
    void ResetMouseDelta();
    
    bool IsMouseButtonPressed(int button) const;
    
    float GetScrollOffset() const { return scrollOffsetY; }
    void ResetScrollOffset() { scrollOffsetY = 0.0f; scrollOffsetX = 0.0f; }
    
    static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
};

#endif