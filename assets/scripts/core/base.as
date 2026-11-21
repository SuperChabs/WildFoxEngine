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

float clamp(float v, float lo, float hi)
{
    return v < lo ? lo : (v > hi ? hi : v);
}

float clamp01(float v)
{
    return clamp(v, 0.0f, 1.0f);
}

float lerp(float a, float b, float t)
{
    return a + (b - a) * t;
}

vec3 lerp(const vec3 &in a, const vec3 &in b, float t)
{
    return vec3(lerp(a.x, b.x, t), lerp(a.y, b.y, t), lerp(a.z, b.z, t));
}

float dot(const vec3 &in a, const vec3 &in b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

float length(const vec3 &in v)
{
    return sqrt(dot(v, v));
}

vec3 normalize(const vec3 &in v)
{
    float len = length(v);
    if (len < EPSILON)
        return vec3(0, 0, 0);
    return vec3(v.x / len, v.y / len, v.z / len);
}
