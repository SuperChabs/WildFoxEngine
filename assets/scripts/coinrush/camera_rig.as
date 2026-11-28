#include "../core/base.as"

// Coin Rush camera rig: put it on any entity, it keeps "Game Camera" behind the player.
// The camera yaw is the player's heading (player.as turns it), the rig only moves the camera
// behind that heading and tilts it to look at the player.
// A script on the camera itself would be lost: scene loading copies the saved camera data
// into the persistent "Game Camera" and destroys the loaded camera entity.

string cameraName = "Game Camera";
string targetName = "player";

float followDistance = 8.0f; // behind the player
float followHeight   = 7.0f; // above the player

// point above the target the camera looks at
float lookHeight = 0.5f;

// higher = snappier; 0 = no smoothing
float followSmoothing = 6.0f;

uint64 cam;
uint64 target;

void OnStart()
{
    cam    = GetEntityByName(cameraName);
    target = GetEntityByName(targetName);
}

void OnUpdate(float dt)
{
    // either entity may be spawned or loaded later
    if (!IsValidEntity(cam))    { cam    = GetEntityByName(cameraName); return; }
    if (!IsValidEntity(target)) { target = GetEntityByName(targetName); return; }

    Transform@ c = GetTransform(cam);
    Transform@ t = GetTransform(target);
    CameraOrientation@ orient = GetCameraOrientation(cam);
    if (c is null || t is null || orient is null) return;

    vec3 targetPos = t.position;

    // follow: stay behind the heading
    // everything is per component: engine functions returning vec3 (normalize) give garbage
    float yawRad = orient.yaw * DEG2RAD;
    vec3 desired = vec3(targetPos.x - cos(yawRad) * followDistance,
                        targetPos.y + followHeight,
                        targetPos.z - sin(yawRad) * followDistance);

    float k = followSmoothing > 0.0f ? clamp01(followSmoothing * dt) : 1.0f;
    c.position = lerp(c.position, desired, k);

    // tilt to look at the target; yaw is left alone, it is the heading
    vec3 camPos = c.position;
    float dx = targetPos.x - camPos.x;
    float dy = targetPos.y + lookHeight - camPos.y;
    float dz = targetPos.z - camPos.z;

    float horizontal = sqrt(dx * dx + dz * dz);
    if (horizontal < EPSILON && abs(dy) < EPSILON) return;

    orient.pitch = clamp(atan2(dy, horizontal) * RAD2DEG, -89.0f, 89.0f);
}
