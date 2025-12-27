#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include "scene/Model.h"
#include "scene/Transform.h"
#include <memory>
#include <vector>

struct SceneObject 
{
    Model* model;
    Transform transform;
    bool isActive;
    
    SceneObject(Model* mdl) 
        : model(mdl), isActive(true) {}
        
    SceneObject(Model* mdl, const Transform& trans) 
        : model(mdl), transform(trans), isActive(true) {}
};

class SceneManager 
{
private:
    std::vector<std::unique_ptr<SceneObject>> objects;
    
public:
    SceneManager() = default;
    ~SceneManager() = default;
    
    SceneObject* AddObject(Model* model);
    SceneObject* AddObject(Model* model, const Transform& transform);
    
    void RemoveObject(SceneObject* object);
    void Clear();
    
    void RenderAll(Shader& shader);
    void Update(float deltaTime);
    
    const std::vector<std::unique_ptr<SceneObject>>& GetObjects() const { return objects; }
    size_t GetObjectCount() const { return objects.size(); }
};

#endif