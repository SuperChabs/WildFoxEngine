#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include "scene/Model.h"
#include "scene/Transform.h"

#include <memory>
#include <vector>

struct SceneObject 
{
    std::string name;

    Model* model;
    Transform transform;

    uint64_t ID;
    bool isActive;

    static inline uint64_t counter = 0;
    
    SceneObject(const std::string& objName, Model* mdl) 
        : name(objName), model(mdl), isActive(true), ID(counter++) {}
        
    SceneObject(const std::string& objName, Model* mdl, const Transform& trans)  
        : name(objName), model(mdl), transform(trans), isActive(true), ID(counter++) {}
};

class SceneManager 
{
private:
    std::vector<std::unique_ptr<SceneObject>> objects;
    std::vector<std::unique_ptr<SceneObject>>::iterator it;
    
public:
    SceneManager() = default;
    ~SceneManager() = default;
    
    SceneObject* AddObject(const std::string& name, std::unique_ptr<Model> model);
    SceneObject* AddObject(const std::string& name, std::unique_ptr<Model> model, const Transform& transform);
    
    void RemoveObject(SceneObject* object);
    void Clear();
    
    void RenderAll(Shader& shader);
    void Update(float deltaTime);
    
    std::vector<std::unique_ptr<SceneObject>>& GetObjects() { return objects; }
    size_t GetObjectCount() const { return objects.size(); }
};

#endif