-- Простий скрипт для обертання об'єкта
local rotationSpeed = 2.0

function onCreate()
    print("Rotation script started")
end

function onUpdate()
    local transform = GetTransform(entity)
    if transform then
        transform.rotation.y = transform.rotation.y + rotationSpeed * deltaTime
    end
end
