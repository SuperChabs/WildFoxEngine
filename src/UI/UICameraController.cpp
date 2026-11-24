#include "UICameraController.h"

#include <cmath>

#include "imgui.h"
#include "ImGuizmo.h"
#include "core/Input.h"

namespace {
    constexpr float kOrbitSensitivity = 0.3f;   // degrees per pixel
    constexpr float kPanSensitivity   = 0.0015f; // world units per pixel, scaled by distance
    constexpr float kDollySensitivity = 0.01f;
    constexpr float kScrollStep       = 0.15f;  // fraction of distance per wheel notch
    constexpr float kMinDistance      = 0.1f;
    constexpr float kMaxDistance      = 1000.0f;
    constexpr float kMaxPitch         = 89.0f;
}

void UICameraController::Update(const bool viewportHovered) {
    const CameraOperation op = GetOperation(viewportHovered);

    if (op != m_operation) {
        m_operation = op;

        if (m_operation != CameraOperation::NONE)
            BeginOperation();
    }

    const glm::vec2 delta = m_input->GetMouseDelta();
    m_input->ResetMouseDelta();

    switch (m_operation) {
        case CameraOperation::ORBIT: Orbit(delta); break;
        case CameraOperation::DOLLY: Dolly(delta); break;
        case CameraOperation::PAN:   Pan(delta);   break;
        default: break;
    }

    if (viewportHovered && m_operation == CameraOperation::NONE)
        ProcessScroll();

    Input::ResetScrollOffset();
}

CameraOperation UICameraController::GetOperation(const bool viewportHovered) const {
    if (!m_input->IsMouseButtonPressed(Key::KEY_MOUSE_MIDDLE))
        return CameraOperation::NONE;

    // an operation may only start over the viewport, but keeps going while the button is held
    if (m_operation == CameraOperation::NONE && (!viewportHovered || ImGuizmo::IsUsingAny()))
        return CameraOperation::NONE;

    if (m_input->IsKeyDown(Key::KEY_LEFT_SHIFT) || m_input->IsKeyDown(Key::KEY_RIGHT_SHIFT))
        return CameraOperation::PAN;

    if (m_input->IsKeyDown(Key::KEY_LEFT_CONTROL) || m_input->IsKeyDown(Key::KEY_RIGHT_CONTROL))
        return CameraOperation::DOLLY;

    return CameraOperation::ORBIT;
}

void UICameraController::ProcessScroll() const {
    const double scrollOffset = Input::GetScrollOffset();
    if (scrollOffset == 0.0)
        return;

    SyncPivot();

    const float factor = std::pow(1.0f - kScrollStep, static_cast<float>(scrollOffset));
    m_editorCam->distance = glm::clamp(m_editorCam->distance * factor, kMinDistance, kMaxDistance);

    UpdateCameraTransform();
}

void UICameraController::BeginOperation() const {
    // skip the delta accumulated before the operation started, otherwise the camera "jumps"
    m_input->ResetMouseDelta();

    // the view cube / entity focus move the camera directly, so re-derive the pivot from it
    SyncPivot();
}

void UICameraController::SyncPivot() const {
    m_editorCam->pivot = m_editorCam->transform.position
                         + m_editorCam->orientation.GetFront() * m_editorCam->distance;
}

void UICameraController::Orbit(const glm::vec2 delta) const {
    auto &orientation = m_editorCam->orientation;

    // Blender turntable: the scene follows the mouse as if grabbed
    orientation.yaw   += delta.x * kOrbitSensitivity;
    orientation.pitch  = glm::clamp(orientation.pitch + delta.y * kOrbitSensitivity, -kMaxPitch, kMaxPitch);

    UpdateCameraTransform();
}

void UICameraController::Pan(const glm::vec2 delta) const {
    // scale by distance so panning feels consistent at any zoom level
    const float sensitivity = m_editorCam->distance * kPanSensitivity;

    const glm::vec3 right = m_editorCam->orientation.GetRight();
    const glm::vec3 up    = m_editorCam->orientation.GetUp();

    m_editorCam->pivot += right * (-delta.x * sensitivity) + up * (-delta.y * sensitivity);

    UpdateCameraTransform();
}

void UICameraController::Dolly(const glm::vec2 delta) const {
    const float factor = 1.0f - delta.y * kDollySensitivity;

    m_editorCam->distance = glm::clamp(m_editorCam->distance * factor, kMinDistance, kMaxDistance);

    UpdateCameraTransform();
}

void UICameraController::UpdateCameraTransform() const {
    const glm::vec3 front = m_editorCam->orientation.GetFront();
    m_editorCam->transform.position = m_editorCam->pivot - front * m_editorCam->distance;
}
