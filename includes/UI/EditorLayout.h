#ifndef EDITOR_LAYOUT_H
#define EDITOR_LAYOUT_H

#include "scene/SceneManager.h"
#include "core/Camera.h"
#include "rendering/Renderer.h"
#include "rendering/Framebuffer.h"  

#include <imgui.h>

#include <functional>
#include <memory>

class EditorLayout 
{
private:
    SceneObject* selectedObject;
    bool showHierarchy;
    bool showInspector;
    bool showProperties;
    bool showConsole;
    
    ImVec2 viewportSize;
    ImVec2 viewportPos;
    bool isViewportHovered;
    bool isViewportFocused;
    
    std::unique_ptr<Framebuffer> framebuffer;  // Додали
    
    void RenderControlPanel(std::function<void()> onCreateCube);
    void RenderSceneHierarchy(SceneManager* sceneManager);
    void RenderInspector();
    void RenderProperties(Camera* camera, Renderer* renderer);
    void RenderConsole();
    void RenderMenuBar(std::function<void()> onCreateCube);
    void RenderViewport();

public:
    EditorLayout();
    
    void BeginFrame();
    void EndFrame();
    
    void RenderEditor(SceneManager* sceneManager, Camera* camera, Renderer* renderer,
                      std::function<void()> onCreateCube);
    
    ImVec2 GetViewportSize() const;
    ImVec2 GetViewportPos() const;
    bool IsViewportHovered() const { return isViewportHovered; }
    bool IsViewportFocused() const { return isViewportFocused; }
    SceneObject* GetSelectedObject() const { return selectedObject; }
    
    Framebuffer* GetFramebuffer() const { return framebuffer.get(); }  // Додали
    
    void SetSelectedObject(SceneObject* obj) { selectedObject = obj; }
};

#endif  