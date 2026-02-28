// player.as

uint64 entity;

void _SetEntity(uint64 e)
{
    entity = e;
}

void OnStart()
{
    Transform@ t = GetTransform(entity);
    if (t is null) return;
}

void OnUpdate(float dt)
{
    Transform@ t = GetTransform(entity);
    if (t is null) return;

    if (IsKeyPressed(KEY_W))
        t.position.z -= 5.0f * dt;

    if (IsKeyPressed(KEY_S))
        t.position.z += 5.0f * dt;

    if (IsKeyPressed(KEY_A))
        t.position.x -= 5.0f * dt;

    if (IsKeyPressed(KEY_D))
        t.position.x += 5.0f * dt;
}

void OnStop()
{
}