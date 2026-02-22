local speed = 5.0
local rotationSpeed = 2.0 

function onCreate()
    print("Cube controller initialized!")
end

function onUpdate()
    local transform = GetTransform(entity)
    
    if transform == nil then
        print("Error: No transform component found")
        return
    end
    
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
    
    if IsKeyPressed(Key.Q) then
        transform.position.y = transform.position.y - speed * deltaTime
    end
    
    if IsKeyPressed(Key.E) then
        transform.position.y = transform.position.y + speed * deltaTime
    end
    
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
    
    if IsKeyJustPressed(Key.SPACE) then
        print("Position: (" .. transform.position.x .. ", " .. transform.position.y .. ", " .. transform.position.z .. ")")
    end
end
