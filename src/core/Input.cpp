#include "core/Input.h"
#include "utils/Logger.h"

float Input::scrollOffsetX = 0.0f;
float Input::scrollOffsetY = 0.0f;

Input::Input(GLFWwindow* win)
    : window(win), lastMouseX(0.0f), lastMouseY(0.0f), 
      firstMouse(true), mouseDeltaX(0.0f), mouseDeltaY(0.0f)
{
}

bool Input::IsKeyPressed(int key) const
{
    return glfwGetKey(window, key) == GLFW_PRESS;

    Logger::Info("Key " + std::to_string(key) + " pressed");
}

bool Input::IsKeyReleased(int key) const
{
    return glfwGetKey(window, key) == GLFW_RELEASE;

    Logger::Info("Key " + std::to_string(key) + " released");
}

bool Input::IsKeyDown(int key) const
{
    return glfwGetKey(window, key) == GLFW_PRESS;

    Logger::Info("Key " + std::to_string(key) + " is down");
}

bool Input::IsKeyJustPressed(int key) const
{
    bool currentState = IsKeyPressed(key);
    bool previousState = keyStates[key];

    if (keyStates.contains(key)) 
        previousState = keyStates.at(key);

    keyStates[key] = currentState;

    return currentState && !previousState;
}

void Input::UpdateMousePosition(double xpos, double ypos)
{
    if (firstMouse)
    {
        lastMouseX = xpos;
        lastMouseY = ypos;
        firstMouse = false;
    }
    
    mouseDeltaX = xpos - lastMouseX;
    mouseDeltaY = lastMouseY - ypos;
    
    lastMouseX = xpos;
    lastMouseY = ypos;
}

glm::vec2 Input::GetMouseDelta()
{
    return glm::vec2(mouseDeltaX, mouseDeltaY);
}

void Input::ResetMouseDelta()
{
    mouseDeltaX = 0.0f;
    mouseDeltaY = 0.0f;
}

bool Input::IsMouseButtonPressed(int button) const
{
    return glfwGetMouseButton(window, button) == GLFW_PRESS;
}

void Input::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    scrollOffsetX += xoffset;
    scrollOffsetY += yoffset;
}