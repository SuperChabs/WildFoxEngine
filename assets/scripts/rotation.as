uint64 entity;
Transform@ t = GetTransform(entity);

void _SetEntity(uint64 e)
{
    entity = e;
}

void OnStart()
{
    if (t is null) return;
}

void OnUpdate(float deltaTime)
{
    if (t is null) return;
    
    t.rotation.y += 90.0f * deltaTime;
}

void OnStop()
{
}