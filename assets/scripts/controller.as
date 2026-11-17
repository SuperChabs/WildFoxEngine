uint64 entity;

void _SetEntity(uint64 e)
{
    entity = e;
}

void OnStart()
{
}

void OnUpdate(float dt)
{
    Transform@ t = GetTransform(entity);
    if (t is null) return;

    vec3 dir = vec3(0, 0, 0);

    if (IsKeyDown(KEY_W))
    {
        t.position.z -= 5.0f * dt;
        dir.z -= 1;
    }

    if (IsKeyDown(KEY_S))
    {
        t.position.z += 5.0f * dt;
        dir.z += 1;
    }

    if (IsKeyDown(KEY_A))
    {
        t.position.x -= 5.0f * dt;
        dir.x -= 1;
    }

    if (IsKeyDown(KEY_D))
    {
        t.position.x += 5.0f * dt;
        dir.x += 1;
    }

    if (IsKeyDown(KEY_SPACE)) {
        t.position.y += 5.0f;
    }
}