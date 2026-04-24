uint64 entity;
uint64 flashlight;

float angle = 0.0f;
float radius = 1.3f;

void _SetEntity(uint64 e)
{
    entity = e;
}

void OnStart()
{
    flashlight = GetEntityByName("Flashlight");
}

void OnUpdate(float dt)
{
    Transform@ t = GetTransform(entity);
    if (t is null) return;

    vec3 dir = vec3(0, 0, 0);

    if (IsKeyPressed(KEY_W))
    {
        t.position.z -= 5.0f * dt;
        dir.z -= 1;
    }

    if (IsKeyPressed(KEY_S))
    {
        t.position.z += 5.0f * dt;
        dir.z += 1;
    }

    if (IsKeyPressed(KEY_A))
    {
        t.position.x -= 5.0f * dt;
        dir.x -= 1;
    }

    if (IsKeyPressed(KEY_D))
    {
        t.position.x += 5.0f * dt;
        dir.x += 1;
    }

    Transform@ ft = GetTransform(flashlight);
    if (ft is null) return;

    float len = sqrt(dir.x * dir.x + dir.z * dir.z);

    if (len > 0)
    {
        dir.x /= len;
        dir.z /= len;

        float targetAngle = atan2(dir.x, -dir.z);

        float diff = targetAngle - angle;

        while (diff > 3.14159f) diff -= 6.28318f;
        while (diff < -3.14159f) diff += 6.28318f;

        angle += diff * 10.0f * dt;

        ft.position.x = t.position.x + sin(angle) * radius;
        ft.position.y = t.position.y + 0.5f;
        ft.position.z = t.position.z - cos(angle) * radius;

        ft.rotation = dir;
    }
}