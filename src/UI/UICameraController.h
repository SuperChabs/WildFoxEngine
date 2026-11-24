/**
 * @file UICameraControl.h
 * @brief Blender-like orbit camera for the editor viewport
 *
 * @author SuperChabs
 * @date 23.11.26
 */

#pragma once
#include "core/Input.h"
#include "UI/EditorCamera.h"

enum class CameraOperation {
    NONE,
    ORBIT,
    PAN,
    DOLLY
};

/**
 * @brief Editor camera controls (Blender style):
 * MMB - orbit, Shift+MMB - pan, Ctrl+MMB - dolly, wheel - zoom
 */
class UICameraController {
    CameraOperation m_operation = CameraOperation::NONE;

    EditorCamera *m_editorCam;
    Input *m_input;

public:
    UICameraController(Input &input, EditorCamera &editorCam)
        : m_editorCam(&editorCam), m_input(&input) {}

    /// @param viewportHovered whether the mouse is over the editor viewport
    void Update(bool viewportHovered);

    [[nodiscard]] CameraOperation GetOperation(bool viewportHovered) const;

private:
    void ProcessScroll() const;

    void BeginOperation() const;
    void SyncPivot() const;

    void Orbit(glm::vec2 delta) const;
    void Pan(glm::vec2 delta) const;
    void Dolly(glm::vec2 delta) const;

    void UpdateCameraTransform() const;
};
