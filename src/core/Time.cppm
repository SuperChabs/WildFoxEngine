module;

#include <GLFW/glfw3.h>

export module WFE.Core.Time;

export class Time 
{
private:
    float currentFrame;
    float lastFrame;
    float deltaTime;
    float timeScale;

public:
    Time()
        : currentFrame(0.0f), lastFrame(0.0f), deltaTime(0.0f), timeScale(1.0f)
    {}
    
    void Update()
    {
        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
    }
    
    float GetDeltaTime() const { return deltaTime * timeScale; }
    float GetTime() const { return currentFrame; }
    float GetTimeScale() const { return timeScale; }
    void SetTimeScale(float scale) { timeScale = scale; }
    
    float GetFPS() const { return 1.0f / deltaTime; }
};