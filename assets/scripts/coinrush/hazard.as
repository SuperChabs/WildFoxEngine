#include "../core/base.as"

// Coin Rush hazard: patrols back and forth and sends the player back to spawn.
// Scripts have no per-entity parameters, so the axis comes from the start position:
// a hazard placed near the X axis patrols along X, otherwise along Z.

float amplitude = 6.0f;
float speed     = 1.2f;

uint64 player;
uint64 spawn;
vec3   basePos;
bool   alongX = true;
float  time   = 0.0f;

void OnStart()
{
    player = GetEntityByName("player");
    spawn  = GetEntityByName("spawn");

    Transform@ t = GetSelfTransform();
    if (t is null) return;

    basePos = t.position;
    alongX  = abs(basePos.x) <= abs(basePos.z);
    time    = basePos.x - basePos.z;
}

void OnUpdate(float dt)
{
    Transform@ t = GetSelfTransform();
    if (t is null) return;

    time += dt;
    float offset = sin(time * speed) * amplitude;

    if (alongX)
        t.position.x = offset;
    else
        t.position.z = offset;
}

void OnTriggerEnter(uint64 other)
{
    if (other != player) return;

    Transform@ p = GetTransform(player);
    Transform@ s = GetTransform(spawn);
    if (p is null || s is null) return;

    p.position = s.position;
    Log("Ouch! Back to spawn");
}
