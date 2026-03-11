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

void OnUpdate(float dt)
{
    if (t is null) return;

    t.rotation.x = dt * 2;
}

void OnStop()
{
}