#include "../core/base.as"

float moveSpeed = 5.0f;
float jumpStep  = 5.0f;

void OnUpdate(float dt)
{
    Transform@ t = GetSelfTransform();
    if (t is null) return;

    if (IsKeyDown(KEY_W)) t.position.z -= moveSpeed * dt;
    if (IsKeyDown(KEY_S)) t.position.z += moveSpeed * dt;
    if (IsKeyDown(KEY_A)) t.position.x -= moveSpeed * dt;
    if (IsKeyDown(KEY_D)) t.position.x += moveSpeed * dt;

    if (IsKeyDown(KEY_SPACE))
        t.position.y += jumpStep;
}
