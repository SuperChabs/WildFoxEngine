local speed = 50000.0

function onCreate()
    print("Rotate script initialized!")
end

function onUpdate()
    -- This will be called every frame
    -- deltaTime is available as a global variable
    if not IsValid(entity) then
        return
    end

    local t = GetTransform(entity)
    if t == nil then
        return
    end

    t.rotation.y = t.rotation.y + speed * deltaTime
end

function onDestroy()
    print("Rotate script destroyed!")
end