#include "../core/base.as"

// Moves the entity in a circle and bobs it up and down.
// Put it on a parent: its children should follow, which tests hierarchy transforms.

float radius    = 1.5f;
float speed     = 1.0f;
float bobHeight = 0.5f;

vec3  center;
float time = 0.0f;

void OnStart()
{
    Transform@ t = GetSelfTransform();
    if (t is null) return;

    center = t.position;
}

void OnUpdate(float dt)
{
    Transform@ t = GetSelfTransform();
    if (t is null) return;

    time += dt;
    t.position = vec3(center.x + cos(time * speed) * radius,
                      center.y + sin(time * speed * 2.0f) * bobHeight,
                      center.z + sin(time * speed) * radius);
}
