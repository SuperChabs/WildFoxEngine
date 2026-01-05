#ifndef TIME_H
#define TIME_H

class Time 
{
private:
    float currentFrame;
    float lastFrame;
    float deltaTime;
    float timeScale;

public:
    Time();
    
    void Update();
    
    float GetDeltaTime() const { return deltaTime * timeScale; }
    float GetTime() const { return currentFrame; }
    float GetTimeScale() const { return timeScale; }
    void SetTimeScale(float scale) { timeScale = scale; }
    
    float GetFPS() const { return 1.0f / deltaTime; }
};

#endif