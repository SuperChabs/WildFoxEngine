#include "core/Input.h"
#include "utils/Logger.h"
#include "magic_enum.hpp"

float Input::scrollOffsetX = 0.0f;
float Input::scrollOffsetY = 0.0f;

Input::Input(GLFWwindow* win)
    : window(win), lastMouseX(0.0f), lastMouseY(0.0f), 
      firstMouse(true), mouseDeltaX(0.0f), mouseDeltaY(0.0f)
{}

bool Input::IsKeyPressed(XKey key) const 
{ 
    bool pressed = glfwGetKey(window, static_cast<int>(key)) == GLFW_PRESS;

#   ifdef DEBUG
    if (pressed)
        Logger::Log(LogLevel::DEBUG, "Key " + std::string(magic_enum::enum_name(key)) + " is pressed");
#   endif

    return pressed;
}

bool Input::IsKeyReleased(XKey key) const 
{ 
    bool released = glfwGetKey(window, static_cast<int>(key)) == GLFW_RELEASE;

#   ifdef DEBUG
    if (released)
        Logger::Log(LogLevel::DEBUG, "Key " + std::string(magic_enum::enum_name(key)) + " is released");
#   endif

    return released;
}

bool Input::IsKeyDown(XKey key) const 
{
    bool down = glfwGetKey(window, static_cast<int>(key)) == GLFW_KEY_DOWN;

#   ifdef DEBUG
    if (down)
        Logger::Log(LogLevel::DEBUG, "Key " + std::string(magic_enum::enum_name(key)) + " is pressed");
#   endif

    return down;
}

bool Input::IsKeyJustPressed(XKey key) const
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