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

    if (t.position.x < 100)
        t.position.x = dt * 2;
    if (t.position.x > 10)
        t.position.x = dt / 2;
}

void OnStop()
{
}