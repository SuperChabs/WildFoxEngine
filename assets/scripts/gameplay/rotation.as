#include "../core/base.as"

float speedY = 90.0f;
float speedX = 10.0f;

void OnUpdate(float dt)
{
    Transform@ t = GetSelfTransform();
    if (t is null) return;

    t.rotation.y += speedY * dt;
    t.rotation.x += speedX * dt;
}
