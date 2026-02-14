-- Скрипт для керування кубом за допомогою клавіатури
-- WASD - рух по осях X та Z
-- Q/E - рух вгору/вниз (Y)
-- Стрілки - поворот куба

local speed = 5.0  -- швидкість руху
local rotationSpeed = 2.0  -- швидкість обертання

function onCreate()
    print("Cube controller initialized!")
end

function onUpdate()
    local transform = GetTransform(entity)
    
    if transform == nil then
        print("Error: No transform component found")
        return
    end
    
    -- Переміщення по осях
    if IsKeyPressed(Key.W) then
        transform.position.z = transform.position.z - speed * deltaTime
    end
    
    if IsKeyPressed(Key.S) then
        transform.position.z = transform.position.z + speed * deltaTime
    end
    
    if IsKeyPressed(Key.A) then
        transform.position.x = transform.position.x - speed * deltaTime
    end
    
    if IsKeyPressed(Key.D) then
        transform.position.x = transform.position.x + speed * deltaTime
    end
    
    -- Вертикальне переміщення
    if IsKeyPressed(Key.Q) then
        transform.position.y = transform.position.y - speed * deltaTime
    end
    
    if IsKeyPressed(Key.E) then
        transform.position.y = transform.position.y + speed * deltaTime
    end
    
    -- Обертання за стрілками
    if IsKeyPressed(Key.UP) then
        transform.rotation.x = transform.rotation.x + rotationSpeed * deltaTime
    end
    
    if IsKeyPressed(Key.DOWN) then
        transform.rotation.x = transform.rotation.x - rotationSpeed * deltaTime
    end
    
    if IsKeyPressed(Key.LEFT) then
        transform.rotation.y = transform.rotation.y + rotationSpeed * deltaTime
    end
    
    if IsKeyPressed(Key.RIGHT) then
        transform.rotation.y = transform.rotation.y - rotationSpeed * deltaTime
    end
    
    -- Друк поточної позиції при натисканні SPACE
    if IsKeyJustPressed(Key.SPACE) then
        print("Position: (" .. transform.position.x .. ", " .. transform.position.y .. ", " .. transform.position.z .. ")")
    end
end
