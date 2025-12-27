#include "scene/SceneManager.h"

SceneObject* SceneManager::AddObject(Model* model)
{
    objects.push_back(std::make_unique<SceneObject>(model));
    return objects.back().get();
}

SceneObject* SceneManager::AddObject(Model* model, const Transform& transform)
{
    objects.push_back(std::make_unique<SceneObject>(model, transform));
    return objects.back().get();
}

void SceneManager::RemoveObject(SceneObject* object)
{
    objects.erase(
        std::remove_if(objects.begin(), objects.end(),
            [object](const std::unique_ptr<SceneObject>& obj) {
                return obj.get() == object;
            }),
        objects.end()
    );
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
    // Тут можна додати логіку оновлення для об'єктів сцени
    // Наприклад: фізика, анімації, скрипти
    for (auto& object : objects)
    {
        if (!object->isActive)
            continue;
        
        // Оновлення об'єктів (якщо потрібно)
    }
}