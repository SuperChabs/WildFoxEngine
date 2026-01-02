#include "scene/SceneManager.h"

#include <algorithm>

SceneObject* SceneManager::AddObject(const std::string& name, std::unique_ptr<Model> model)
{
    auto obj = std::make_unique<SceneObject>(name, std::move(model));
    SceneObject* ptr = obj.get();
    
    objects[ptr->objectID] = std::move(obj);
    
    return ptr;
}

SceneObject* SceneManager::AddObject(const std::string& name, std::unique_ptr<Model> model, const Transform& transform)
{
    auto obj = std::make_unique<SceneObject>(name, std::move(model), transform);
    SceneObject* ptr = obj.get();
    
    objects[ptr->objectID] = std::move(obj);
    
    return ptr;
}

void SceneManager::RemoveObject(SceneObject* object)
{
    if (!object)
        return;
        
    auto it = objects.find(object->objectID);
    
    if (it != objects.end()) 
        objects.erase(it);
}

void SceneManager::RemoveObject(uint64_t id) 
{
    auto it = objects.find(id);
    if (it != objects.end())
        objects.erase(it);
}

void SceneManager::Clear()
{
    objects.clear();
}

void SceneManager::RenderAll(Shader& shader)
{
    for (auto& [id, object] : objects)
    {
        if (!object->isActive)
            continue;
            
        shader.setMat4("model", object->transform.GetModelMatrix());
        object->model->Draw(shader);
    }
}

void SceneManager::Update(float deltaTime)
{
    for (auto& [id, object] : objects)
        if (!object->isActive)
            continue;
}