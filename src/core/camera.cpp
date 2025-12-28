#include "core/Camera.h"
#include "utils/Logger.h"

using namespace glm;

Camera::Camera(vec3 position, vec3 up, float yaw, float pitch)
    : front(vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM)
{
    this->position = position;
    this->worldUp = up;
    this->yaw = yaw;
    this->pitch = pitch;
    UpdateCameraVectors();

    Logger::Log(LogLevel::INFO, "Camera created at position (" + std::to_string(position.x) + ", " + std::to_string(position.y) + ", " + std::to_string(position.z) + ")");
}

Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
    : front(vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM)
{
    position = vec3(posX, posY, posZ);
    worldUp = vec3(upX, upY, upZ);
    this->yaw = yaw;
    this->pitch = pitch;
    UpdateCameraVectors();

    Logger::Log(LogLevel::INFO, "Camera created at position (" + std::to_string(posX) + ", " + std::to_string(posY) + ", " + std::to_string(posZ) + ")"); 
}

void Camera::ProcessKeyboard(CameraMovement direction, float deltaTime)
{
    float velocity = movementSpeed * deltaTime;
    // При движении вперед/назад игнорируем вертикальную компоненту front,
    // чтобы не подниматься/опускаться при взгляде вверх/вниз
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

void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    yaw   += xoffset;
    pitch += yoffset;

    // Убеждаемся, что когда тангаж выходит за пределы обзора, экран не переворачивается
    if (constrainPitch)
    {
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;
    }

    if (yaw > 360.0f || yaw < -360.0f)
        yaw = 0.0f;

    // Обновляем значения вектора-прямо, вектора-вправо и вектора-вверх, используя обновленные значения углов Эйлера
    UpdateCameraVectors();
}

void Camera::ProcessScrool(float yoffset)
{
    if (zoom > 1.0f && zoom <= 55.0f)
        zoom -= yoffset;
    else if (zoom <= 1.0f)
        zoom = 1.1f;
    else if (zoom >= 55.0f)
        zoom = 55.0f;
}

void Camera::UpdateCameraVectors()
{
    // Вычисляем новый вектор-прямо
    vec3 front;
    front.x = cos(radians(yaw)) * cos(radians(pitch));
    front.y = sin(radians(pitch));
    front.z = sin(radians(yaw)) * cos(radians(pitch));
    this->front = normalize(front);
    // Также пересчитываем вектор-вправо и вектор-вверх
    right = normalize(cross(this->front, worldUp));  // Нормализуем векторы, потому что их длина становится стремится к 0 тем больше, чем больше вы смотрите вверх или вниз, что приводит к более медленному движению.
    up    = normalize(cross(right, this->front));
}
