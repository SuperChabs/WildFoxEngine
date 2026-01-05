module;

#include <memory>
#include <map>

#include <glm/glm.hpp>

export module XEngine.Scene.SceneManager;

import XEngine.Scene.Model;
import XEngine.Scene.Transform;
import XEngine.Core.Shader;

export struct ObjectParams 
{
    bool autoRotate = false;
    float rotateSpeed = 50.0f; 

    glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
    bool useColor = true;
    std::string texturePath = "";
};

export struct SceneObject 
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

export class SceneManager 
{
private:
    std::map<uint64_t, std::unique_ptr<SceneObject>> objects;
    
public:
    SceneManager() {};
    ~SceneManager() {};
    
    SceneObject* AddObject(const std::string& name, std::unique_ptr<Model> model)
    {
        auto obj = std::make_unique<SceneObject>(name, std::move(model));
        SceneObject* ptr = obj.get();
        
        objects[ptr->objectID] = std::move(obj);
        
        return ptr;
    }

    SceneObject* AddObject(const std::string& name, std::unique_ptr<Model> model, const Transform& transform)
    {
        auto obj = std::make_unique<SceneObject>(name, std::move(model), transform);
        SceneObject* ptr = obj.get();
        
        objects[ptr->objectID] = std::move(obj);
        
        return ptr;
    }
    
    void RemoveObject(SceneObject* object)
    {
        if (!object)
            return;
            
        auto it = objects.find(object->objectID);
        
        if (it != objects.end()) 
            objects.erase(it);
    }

    void RemoveObject(uint64_t id)
    {
        auto it = objects.find(id);
        if (it != objects.end())
            objects.erase(it);
    }

    void Clear()
    {
        objects.clear();
    }
    
    void RenderAll(Shader& shader)
    {
        for (auto& [id, object] : objects)
        {
            if (!object->isActive)
                continue;
                
            shader.setMat4("model", object->transform.GetModelMatrix());
            object->model->Draw(shader);
        }
    }
    
    void Update(float deltaTime)
    {
        for (auto& [id, object] : objects)
            if (!object->isActive)
                continue;
    }
    
    std::map<uint64_t, std::unique_ptr<SceneObject>>& GetObjects() { return objects; }
    size_t GetObjectCount() const { return objects.size(); }
};