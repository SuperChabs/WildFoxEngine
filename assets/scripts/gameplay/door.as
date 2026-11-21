#include "../core/base.as"

// Attach to a trigger: raises the door entity while something is inside.

string doorName   = "door1";
float  openHeight = 5.0f;
float  openSpeed  = 3.0f;

uint64 door;
float  closedY = 0.0f;
float  targetY = 0.0f;
bool   isOpen  = false;

void OnStart()
{
    door = GetEntityByName(doorName);

    Transform@ t = GetTransform(door);
    if (t is null) return;

    closedY = t.position.y;
    targetY = closedY;
}

void OnTriggerEnter(uint64 other)
{
    if (isOpen || other == door) return;
    targetY = closedY + openHeight;
    isOpen  = true;
}

void OnTriggerExit(uint64 other)
{
    if (!isOpen || other == door) return;
    targetY = closedY;
    isOpen  = false;
}

void OnUpdate(float dt)
{
    if (!IsValidEntity(door)) return;

    Transform@ t = GetTransform(door);
    if (t is null) return;

    float diff = targetY - t.position.y;
    if (abs(diff) > 0.001f)
        t.position.y += diff * clamp01(openSpeed * dt);
    else
        t.position.y = targetY;
}
