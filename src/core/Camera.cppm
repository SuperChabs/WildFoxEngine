module;

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <string>

export module WFE.Core.Camera;

import WFE.Core.Logger;

using namespace glm;

export enum CameraMovement 
{
    FORWARD,
    BACKWARD,
    RIGHT,
    LEFT,
    UP,
    DOWN
};

const float YAW         = -95.0f;
const float PITCH       = 0.0f;
const float SPEED       = 3.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM        = 55.0f;

export class Camera 
{
private:
    vec3 position;
    vec3 front;
    vec3 up;
    vec3 right;
    vec3 worldUp;

    float yaw;
    float pitch;

    float movementSpeed;
    float mouseSensitivity;
    float zoom;

public:
    Camera(vec3 position = vec3(0.0f, 0.0f, 0.0f), vec3 up = vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH)
        : front(vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM)
    {
        this->position = position;
        this->worldUp = up;
        this->yaw = yaw;
        this->pitch = pitch;
        UpdateCameraVectors();

        Logger::Log(LogLevel::INFO, "Camera created at position (" + std::to_string(position.x) + ", " + std::to_string(position.y) + ", " + std::to_string(position.z) + ")");
    }

    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
        : front(vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM)
    {
        position = vec3(posX, posY, posZ);
        worldUp = vec3(upX, upY, upZ);
        this->yaw = yaw;
        this->pitch = pitch;
        UpdateCameraVectors();

        Logger::Log(LogLevel::INFO, "Camera created at position (" + std::to_string(posX) + ", " + std::to_string(posY) + ", " + std::to_string(posZ) + ")"); 
    }

    void ProcessKeyboard(CameraMovement direction, float deltaTime)
    {
        float velocity = movementSpeed * deltaTime;
        if (direction == FORWARD)
        {
            vec3 flatFront = vec3(front.x, 0.0f, front.z);
            float len = glm::length(flatFront.x * flatFront.x + flatFront.z * flatFront.z);
            if (len > 1e-6f)
                position += normalize(flatFront) * velocity;
        }
        if (direction == BACKWARD)
        {
            vec3 flatFront = vec3(front.x, 0.0f, front.z);
            float len = glm::length(flatFront.x * flatFront.x + flatFront.z * flatFront.z);
            if (len > 1e-6f)
                position -= normalize(flatFront) * velocity;
        }
        if (direction == LEFT)
            position -= right * velocity;
        if (direction == RIGHT)
            position += right * velocity;
        if (direction == UP)
            position += worldUp * velocity;
        if (direction == DOWN)
            position -= worldUp * velocity;
    }

    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= mouseSensitivity;
        yoffset *= mouseSensitivity;

        yaw   += xoffset;
        pitch += yoffset;

        if (constrainPitch)
        {
            if (pitch > 89.0f)
                pitch = 89.0f;
            if (pitch < -89.0f)
                pitch = -89.0f;
        }

        if (yaw > 360.0f || yaw < -360.0f)
            yaw = 0.0f;

        UpdateCameraVectors();
    }

    void ProcessScrool(float yoffset)
    {
        if (zoom > 1.0f && zoom <= 55.0f)
            zoom -= yoffset;
        else if (zoom <= 1.0f)
            zoom = 1.1f;
        else if (zoom >= 55.0f)
            zoom = 55.0f;
    }

    mat4 GetViewMatrix() const {return glm::lookAt(position, position + front, up);}

    vec3 GetPosition() const {return position;}
    vec3 GetFront()    const {return front;}
    vec3 GetUp()       const {return up;}
    vec3 GetRight()    const {return right;}
    vec3 GetWorldUp()  const {return worldUp;}

    float GetYaw()   const { return yaw; }
    float GetPitch() const { return pitch; }

    float GetMovementSpeed()    const { return movementSpeed; }
    float GetMouseSensitivity() const { return mouseSensitivity; }
    float GetZoom()             const { return zoom; }

    void SetPosition(glm::vec3 newValue) { position = newValue; }
    void SetFront(glm::vec3 newValue)    { front = newValue; }
    void SetUp(glm::vec3 newValue)       { up = newValue; }
    void SetRight(glm::vec3 newValue)    { right = newValue; }
    void SetWorldUp(glm::vec3 newValue)  { worldUp = newValue; }

    void SetYaw(float newValue)   
    { 
        yaw = newValue; 
        if (yaw > 360.0f || yaw < -360.0f) 
            yaw = 0.0f; 
            
        UpdateCameraVectors();   
    }
    void SetPitch(float newValue) 
    { 
        pitch = newValue; 
        if (pitch > 89.0f) 
            pitch = 89.0f; 
        if (pitch < -89.0f) 
            pitch = -89.0f; 

        UpdateCameraVectors(); 
    }

    void SetMovementSpeed(float newValue)    { movementSpeed = newValue; }
    void SetMouseSensitivity(float newValue) { mouseSensitivity = newValue; }
    void SetZoom(float newValue)             { zoom = newValue; }

private:
    void UpdateCameraVectors()
    {
        vec3 front;
        front.x = cos(radians(yaw)) * cos(radians(pitch));
        front.y = sin(radians(pitch));
        front.z = sin(radians(yaw)) * cos(radians(pitch));
        this->front = normalize(front);
        // Также пересчитываем вектор-вправо и вектор-вверх
        right = normalize(cross(this->front, worldUp));  // Нормализуем векторы, потому что их длина становится стремится к 0 тем больше, чем больше вы смотрите вверх или вниз, что приводит к более медленному движению.
        up    = normalize(cross(right, this->front));
    }
};