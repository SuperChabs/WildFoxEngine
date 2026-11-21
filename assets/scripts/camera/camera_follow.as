#include "../core/base.as"

// Attach to a camera entity: follows the target with an offset and always looks at it.

string targetName = "player";

// camera position relative to the target
float offsetX = 0.0f;
float offsetY = 3.0f;
float offsetZ = 5.0f;

// point on the target the camera looks at (e.g. head instead of feet)
float lookHeight = 1.0f;

// higher = snappier; 0 = no smoothing
float followSmoothing = 5.0f;

uint64 target;

void OnStart()
{
    target = GetEntityByName(targetName);
    if (!IsValidEntity(target))
        Log("camera_follow: target '" + targetName + "' not found");
}

void OnUpdate(float dt)
{
    // the target may be spawned or loaded later
    if (!IsValidEntity(target))
    {
        target = GetEntityByName(targetName);
        return;
    }

    Transform@ cam = GetSelfTransform();
    Transform@ tgt = GetTransform(target);
    CameraOrientation@ orient = GetCameraOrientation(self);
    if (cam is null || tgt is null || orient is null) return;

    vec3 targetPos = tgt.position;

    // follow
    vec3 desired = vec3(targetPos.x + offsetX, targetPos.y + offsetY, targetPos.z + offsetZ);
    float t = followSmoothing > 0.0f ? clamp01(followSmoothing * dt) : 1.0f;
    cam.position = lerp(cam.position, desired, t);

    // look at (same convention as c_CameraOrientation::GetFront)
    vec3 camPos = cam.position;
    vec3 lookPoint = vec3(targetPos.x, targetPos.y + lookHeight, targetPos.z);
    vec3 dir = vec3(lookPoint.x - camPos.x, lookPoint.y - camPos.y, lookPoint.z - camPos.z);
    if (length(dir) < EPSILON) return;

    dir = normalize(dir);
    orient.yaw   = atan2(dir.z, dir.x) * RAD2DEG;
    orient.pitch = clamp(asin(dir.y) * RAD2DEG, -89.0f, 89.0f);
}
