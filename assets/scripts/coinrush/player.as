#include "../core/base.as"

// Coin Rush player, tank controls: A/D turn, W/S roll forward/back, LEFT SHIFT to sprint.
// The heading is the yaw of "Game Camera": camera_rig.as keeps the camera behind it.
// Respawns at the "spawn" entity after falling off the arena.

float walkSpeed   = 5.0f;
float sprintSpeed = 9.0f;
float turnSpeed   = 120.0f; // degrees per second

// inner size of the arena walls minus the player radius
float arenaHalfSize = 7.4f;
float killHeight    = -5.0f;

uint64 spawn;
uint64 cam;

void OnStart()
{
    spawn = GetEntityByName("spawn");
    cam   = GetEntityByName("Game Camera");

    if (!IsValidEntity(spawn))
        Log("player: 'spawn' entity not found");
}

void OnUpdate(float dt)
{
    Transform@ t = GetTransform(self);
    CameraOrientation@ heading = GetCameraOrientation(cam);
    if (t is null || heading is null) return;

    if (IsKeyDown(KEY_A)) heading.yaw -= turnSpeed * dt;
    if (IsKeyDown(KEY_D)) heading.yaw += turnSpeed * dt;

    // flat forward vector, same convention as c_CameraOrientation::GetFront
    float yawRad   = heading.yaw * DEG2RAD;
    float forwardX = cos(yawRad);
    float forwardZ = sin(yawRad);

    float speed = IsKeyDown(KEY_LEFT_SHIFT) ? sprintSpeed : walkSpeed;
    float move  = 0.0f;
    if (IsKeyDown(KEY_W)) move += 1.0f;
    if (IsKeyDown(KEY_S)) move -= 1.0f;

    t.position.x += forwardX * move * speed * dt;
    t.position.z += forwardZ * move * speed * dt;

    t.position.x = clamp(t.position.x, -arenaHalfSize, arenaHalfSize);
    t.position.z = clamp(t.position.z, -arenaHalfSize, arenaHalfSize);

    if (t.position.y < killHeight)
        Respawn();
}

void Respawn()
{
    Transform@ t = GetTransform(self);
    Transform@ s = GetTransform(spawn);
    if (t is null || s is null) return;

    t.position = s.position;
}
