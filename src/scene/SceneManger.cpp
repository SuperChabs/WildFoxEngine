#include "scene/SceneManager.h"

#include <algorithm>

SceneObject* SceneManager::AddObject(const std::string& name, std::unique_ptr<Model> model)
{
    objects.insert(ObjectID, std::make_unique<SceneObject>(name, std::move(model)));
    return objects.
}

SceneObject* SceneManager::AddObject(const std::string& name, std::unique_ptr<Model> model, const Transform& transform)
{
    objects.push_back(std::make_unique<SceneObject>(name, std::move(model), transform));
    return objects.back().get();
}

void SceneManager::RemoveObject(SceneObject* object)
{
    return;
}

void SceneManager::Clear()
{
    objects.clear();
}

void SceneManager::RenderAll(Shader& shader)
{
    for (auto& object : objects)
    {
        if (!object->isActive)
            continue;
            
        shader.setMat4("model", object->transform.GetModelMatrix());
        object->model->Draw(shader);
    }
}

void SceneManager::Update(float deltaTime)
{
    for (auto& object : objects)
        if (!object->isActive)
            continue;
}