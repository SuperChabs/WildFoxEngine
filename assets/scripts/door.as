uint64 targetDoor;
vec3 startPos;

bool open = false;
bool initialized = false;

float speed = 5.0f;
float openHeight = 5.0f;

void SetTargetByName(const string &in name)
{
    targetDoor = GetEntityByName(name);
    if (targetDoor == 0) return;

    Transform@ t = GetTransform(targetDoor);
    if (t !is null)
        startPos = t.position;
}

void OnTriggerEnter(uint64 other)
{
    open = true;
}

void OnTriggerExit(uint64 other)
{
    open = false;
}

void OnUpdate(float dt)
{
    if (!initialized)
    {
        SetTargetByName("door1");
        if (targetDoor != 0)
            initialized = true;
    }

    Transform@ t = GetTransform(targetDoor);
    if (t is null) return;

    vec3 targetPos = startPos;
    if (open) targetPos.y += openHeight;

    vec3 delta = targetPos - t.position;

    if ((delta.x*delta.x + delta.y*delta.y + delta.z*delta.z) < 0.001f)
    {
        t.position = targetPos;
    }
    else
    {
        t.position = t.position + delta * speed * dt;
    }
}