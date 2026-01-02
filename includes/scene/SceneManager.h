#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include "scene/Model.h"
#include "scene/Transform.h"

#include <memory>
#include <map>

struct ObjectParams 
{
    bool autoRotate = false;
    float rotateSpeed = 50.0f; 

    glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
    bool useColor = true;
    std::string texturePath = "";
};

struct SceneObject 
{
    std::string name;
    std::unique_ptr<Model> model;
    Transform transform;
    bool isActive; 

    ObjectParams params;
    
    static inline uint64_t counter = 1;
    uint64_t objectID;
    
    SceneObject(const std::string& objName, std::unique_ptr<Model> mdl) 
        : name(objName), model(std::move(mdl)), isActive(true), objectID(counter++) {}
        
    SceneObject(const std::string& objName, std::unique_ptr<Model> mdl, const Transform& trans)  
        : name(objName), model(std::move(mdl)), transform(trans), isActive(true), objectID(counter++) {}
};

class SceneManager 
{
private:
    std::map<uint64_t, std::unique_ptr<SceneObject>> objects;
    
public:
    SceneManager() = default;
    ~SceneManager() = default;
    
    SceneObject* AddObject(const std::string& name, std::unique_ptr<Model> model);
    SceneObject* AddObject(const std::string& name, std::unique_ptr<Model> model, const Transform& transform);
    
    void RemoveObject(SceneObject* object);
    void RemoveObject(uint64_t id);
    void Clear();
    
    void RenderAll(Shader& shader);
    void Update(float deltaTime);
    
    std::map<uint64_t, std::unique_ptr<SceneObject>>& GetObjects() { return objects; }
    size_t GetObjectCount() const { return objects.size(); }
};

#endif