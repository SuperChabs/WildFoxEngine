#include "Time.h"

Time::Time()
    : currentFrame(0.0f), lastFrame(0.0f), deltaTime(0.0f), timeScale(1.0f) {
}

void Time::Update() {
    currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
}

float Time::GetDeltaTime() const {
    return deltaTime * timeScale;
}

float Time::GetTime() const {
    return currentFrame;
}

float Time::GetTimeScale() const {
    return timeScale;
}

void Time::SetTimeScale(float scale) {
    timeScale = scale;
}

float Time::GetFPS() const {
    return 1.0f / deltaTime;
}