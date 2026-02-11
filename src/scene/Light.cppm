module;

#include <glm/glm.hpp>
#include <string>

export module WFE.Scene.Light;

export enum class LightType
{
    DIRECTIONAL,
    POINT,
    SPOT
};

export class Light
{
protected:
    LightType type;
    std::string name;
    
    glm::vec3 position;
    glm::vec3 direction;
    
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    
    float intensity;
    bool isActive;
    bool castShadows;
    
    float constant;
    float linear;
    float quadratic;
    float radius;
    
    float innerCutoff;
    float outerCutoff;

public:
    Light(LightType type, const std::string& name = "Light")
        : type(type)
        , name(name)
        , position(0.0f, 5.0f, 0.0f)
        , direction(0.0f, -1.0f, 0.0f)
        , ambient(0.2f)
        , diffuse(0.8f)
        , specular(1.0f)
        , intensity(1.0f)
        , isActive(true)
        , castShadows(true)
        , constant(1.0f)
        , linear(0.09f)
        , quadratic(0.032f)
        , radius(50.0f)
        , innerCutoff(12.5f)
        , outerCutoff(17.5f)
    {}

    virtual ~Light() = default;

    LightType GetType() const { return type; }
    const std::string& GetName() const { return name; }
    const glm::vec3& GetPosition() const { return position; }
    const glm::vec3& GetDirection() const { return direction; }
    const glm::vec3& GetAmbient() const { return ambient; }
    const glm::vec3& GetDiffuse() const { return diffuse; }
    const glm::vec3& GetSpecular() const { return specular; }
    float GetIntensity() const { return intensity; }
    bool IsActive() const { return isActive; }
    bool CastsShadows() const { return castShadows; }
    float GetConstant() const { return constant; }
    float GetLinear() const { return linear; }
    float GetQuadratic() const { return quadratic; }
    float GetRadius() const { return radius; }
    float GetInnerCutoff() const { return innerCutoff; }
    float GetOuterCutoff() const { return outerCutoff; }

    void SetName(const std::string& n) { name = n; }
    void SetPosition(const glm::vec3& pos) { position = pos; }
    void SetDirection(const glm::vec3& dir) { direction = glm::normalize(dir); }
    void SetAmbient(const glm::vec3& amb) { ambient = amb; }
    void SetDiffuse(const glm::vec3& diff) { diffuse = diff; }
    void SetSpecular(const glm::vec3& spec) { specular = spec; }
    void SetIntensity(float i) { intensity = i; }
    void SetActive(bool active) { isActive = active; }
    void SetCastShadows(bool shadows) { castShadows = shadows; }
    void SetConstant(float c) { constant = c; }
    void SetLinear(float l) { linear = l; }
    void SetQuadratic(float q) { quadratic = q; }
    void SetRadius(float r) { radius = r; }
    void SetInnerCutoff(float angle) { innerCutoff = angle; }
    void SetOuterCutoff(float angle) { outerCutoff = angle; }
    void SetType(LightType newType) { type = newType; }
};

export class DirectionalLight : public Light
{
public:
    DirectionalLight(const std::string& name = "Directional Light")
        : Light(LightType::DIRECTIONAL, name)
    {
        direction = glm::vec3(0.0f, -1.0f, 0.0f);
        ambient = glm::vec3(0.05f);
        diffuse = glm::vec3(0.4f);
        specular = glm::vec3(0.5f);
    }
};

export class PointLight : public Light
{
public:
    PointLight(const std::string& name = "Point Light")
        : Light(LightType::POINT, name)
    {
        position = glm::vec3(0.0f, 5.0f, 0.0f);
        ambient = glm::vec3(0.05f);
        diffuse = glm::vec3(0.8f);
        specular = glm::vec3(1.0f);
        constant = 1.0f;
        linear = 0.09f;
        quadratic = 0.032f;
        radius = 50.0f;
    }
};

export class SpotLight : public Light
{
public:
    SpotLight(const std::string& name = "Spot Light")
        : Light(LightType::SPOT, name)
    {
        position = glm::vec3(0.0f, 5.0f, 0.0f);
        direction = glm::vec3(0.0f, -1.0f, 0.0f);
        ambient = glm::vec3(0.0f);
        diffuse = glm::vec3(1.0f);
        specular = glm::vec3(1.0f);
        constant = 1.0f;
        linear = 0.09f;
        quadratic = 0.032f;
        innerCutoff = 12.5f;
        outerCutoff = 17.5f;
    }
};