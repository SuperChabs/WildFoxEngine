uint64 targetDoor;
float openHeight = 5.0f;
bool isOpen = false;

void SetTargetByName(const string &in name)
{
    targetDoor = GetEntityByName(name);
}

void OnStart()
{
    SetTargetByName("door1");
}

void OnTriggerEnter(uint64 other)
{
    if (isOpen) return;
    if (other == targetDoor) return; 
    if (!IsValidEntity(targetDoor)) return;

    Transform@ t = GetTransform(targetDoor);
    if (t is null) return;

    t.position.y += openHeight;
    isOpen = true;
}

void OnTriggerExit(uint64 other)
{
    if (!isOpen) return;
    if (other == targetDoor) return; 
    if (!IsValidEntity(targetDoor)) return;

    Transform@ t = GetTransform(targetDoor);
    if (t is null) return;

    t.position.y -= openHeight;
    isOpen = false;
}