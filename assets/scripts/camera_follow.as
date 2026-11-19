uint64 targetEntity;
uint64 cameraEntity;

float followDistance = 5.0f;
float followHeight = 3.0f;
float followSideOffset = 0.0f;
float smoothingSpeed = 5.0f;
bool lookAtTarget = true;

void _SetEntity(uint64 e)
{
    cameraEntity = e;
}

void OnStart()
{
    targetEntity = GetEntityByName("player");
}

void OnUpdate(float deltaTime)
{
    Transform@ cameraTrans = GetTransform(cameraEntity);
    Transform@ targetTrans = GetTransform(targetEntity);
    
    if (cameraTrans is null || targetTrans is null)
        return;
    
    vec3 targetPos = targetTrans.position;
    
    vec3 desiredPos;
    desiredPos.x = targetPos.x + followSideOffset;
    desiredPos.y = targetPos.y + followHeight;
    desiredPos.z = targetPos.z + followDistance;
    
    float lerpFactor = smoothingSpeed * deltaTime;
    lerpFactor = lerpFactor > 1.0f ? 1.0f : lerpFactor;
    
    cameraTrans.position.x += (desiredPos.x - cameraTrans.position.x) * lerpFactor;
    cameraTrans.position.y += (desiredPos.y - cameraTrans.position.y) * lerpFactor;
    cameraTrans.position.z += (desiredPos.z - cameraTrans.position.z) * lerpFactor;
    
    if (lookAtTarget)
    {
        vec3 dir = targetPos - cameraTrans.position;
        float len = sqrt(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);

        if (len > 0.0001f)
        {
            dir.x /= len;
            dir.y /= len;
            dir.z /= len;

            float yaw   = atan2(dir.z, dir.x) * RAD2DEG;
            float pitch = asin(dir.y) * RAD2DEG;

            cameraTrans.rotation.x = pitch;
            cameraTrans.rotation.y = yaw;
            cameraTrans.rotation.z = 0.0f;
        }
    }
}
