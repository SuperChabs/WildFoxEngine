// Minimal base for every script: include it first
//     #include "../core/base.as"
//
// Engine callbacks (all optional, declare only the ones you need):
//     void OnStart()
//     void OnUpdate(float dt)
//     void OnStop()
//     void OnTriggerEnter(uint64 other)
//     void OnTriggerExit(uint64 other)

// Entity this script is attached to. Set by the engine before OnStart,
// so don't use it in global variable initializers.
uint64 self;

void _SetEntity(uint64 e)
{
    self = e;
}

Transform@ GetSelfTransform()
{
    return GetTransform(self);
}

// Math --------------------------------------------------------------------------------------------

const float PI      = 3.14159265f;
const float DEG2RAD = PI / 180.0f;
const float RAD2DEG = 180.0f / PI;
const float EPSILON = 0.0001f;

// clamp, lerp(float), dot, length and normalize are registered by the engine (ASRegisterMath.h),
// redefining them here is a build error

float clamp01(float v)
{
    return clamp(v, 0.0f, 1.0f);
}

vec3 lerp(const vec3 &in a, const vec3 &in b, float t)
{
    return vec3(lerp(a.x, b.x, t), lerp(a.y, b.y, t), lerp(a.z, b.z, t));
}
