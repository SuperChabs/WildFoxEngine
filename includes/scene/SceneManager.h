#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include "scene/Model.h"
#include "scene/Transform.h"

#include <memory>
#include <map>

struct SceneObject 
{
    std::string name;
    std::unique_ptr<Model> model;
    Transform transform;
    bool isActive;  
    
    SceneObject(const std::string& objName, std::unique_ptr<Model> mdl) 
        : name(objName), model(std::move(mdl)), isActive(true) {}
        
    SceneObject(const std::string& objName, std::unique_ptr<Model> mdl, const Transform& trans)  
        : name(objName), model(std::move(mdl)), transform(trans), isActive(true) {}
};

class SceneManager 
{
private:
    static inline uint64_t counter = 0;
    uint64_t objectID;

    std::map<uint64_t, SceneObject*> objects;
    
public:
    SceneManager() : objectID(counter++) {};
    ~SceneManager() = default;
    
    SceneObject* AddObject(const std::string& name, std::unique_ptr<Model> model);
    SceneObject* AddObject(const std::string& name, std::unique_ptr<Model> model, const Transform& transform);
    
    void RemoveObject(SceneObject* object);
    void Clear();
    
    void RenderAll(Shader& shader);
    void Update(float deltaTime);
    
    std::map<counter, SceneObject*>& GetObjects() { return objects; }
    size_t GetObjectCount() const { return objects.size(); }
};

#endif