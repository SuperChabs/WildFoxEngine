#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

using namespace glm;

enum CameraMovement 
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

class Camera 
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
    Camera(vec3 position = vec3(0.0f, 0.0f, 0.0f), vec3 up = vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);

    void ProcessKeyboard(CameraMovement direction, float deltaTime);
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
    void ProcessScrool(float yoffset);

    mat4 GetViewMatrix() {return glm::lookAt(position, position + front, up);}

    // Getteri blyat
    vec3 GetPosition() {return position;}
    vec3 GetFront() {return front;}
    vec3 GetUp() {return up;}
    vec3 GetRight() {return right;}
    vec3 GetWorldUp() {return worldUp;}

    float GetYaw() { return yaw; }
    float GetPitch() { return pitch; }

    float GetMovementSpeed() { return movementSpeed; }
    float GetMouseSensitivity() { return mouseSensitivity; }
    float GetZoom() { return zoom; }

    // Setteri nahui
    void SetPosition(glm::vec3 newValue) { position = newValue; }
    void SetFront(glm::vec3 newValue) { front = newValue; }
    void SetUp(glm::vec3 newValue) { up = newValue; }
    void SetRight(glm::vec3 newValue) { right = newValue; }
    void SetWorldUp(glm::vec3 newValue) { worldUp = newValue; }

    void SetYaw(float newValue) { yaw = newValue; }
    void SetPitch(float newValue) { pitch = newValue; }

    void SetMovementSpeed(float newValue) { movementSpeed = newValue; }
    void SetMouseSensitivity(float newValue) { mouseSensitivity = newValue; }
    void SetZoom(float newValue) { zoom = newValue; }

private:
    void UpdateCameraVectors();
};

#endif