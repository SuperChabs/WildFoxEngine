#include "core/Time.h"

#include <GLFW/glfw3.h>

Time::Time()
    : currentFrame(0.0f), lastFrame(0.0f), deltaTime(0.0f), timeScale(1.0f)
{
}

void Time::Update()
{
    currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
}