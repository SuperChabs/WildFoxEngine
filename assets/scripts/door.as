uint64 targetDoor;
float openHeight   = 5.0f;
float targetY      = 4.0f;
float currentY     = 0.0f;
float openSpeed    = 3.0f;
bool  isOpen       = false;
bool  initialized  = false;

void OnStart()
{
    targetDoor = GetEntityByName("door1");
}

void OnTriggerEnter(uint64 other)
{
    if (isOpen || other == targetDoor) return;
    if (!IsValidEntity(targetDoor)) return;

    Transform@ t = GetTransform(targetDoor);
    if (t is null) return;

    if (!initialized)
    {
        currentY   = t.position.y;
        initialized = true;
    }

    targetY = currentY + openHeight;
    isOpen  = true;
}

void OnTriggerExit(uint64 other)
{
    if (!isOpen || other == targetDoor) return;
    targetY = currentY;
    isOpen  = false;
}

void OnUpdate(float dt)
{
    if (!IsValidEntity(targetDoor)) return;

    Transform@ t = GetTransform(targetDoor);
    if (t is null) return;

    float diff = targetY - t.position.y;
    if (abs(diff) > 0.001f)
        t.position.y += diff * openSpeed * dt;
    else
        t.position.y = targetY;
}