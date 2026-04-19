#version 460 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

#define MAX_LIGHTS 8

// material
struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    vec3 color;
};

uniform Material material;
uniform bool useColor;
uniform vec2 tiling;

// light
struct Light {
    int   type; // 0 = directional, 1 = point, 2 = spot

    vec3  position;
    vec3  direction;

    vec3  ambient;
    vec3  diffuse;
    vec3  specular;

    float constant;
    float linear;
    float quadratic;

    float innerCutoff;
    float outerCutoff;
};

uniform Light lights[MAX_LIGHTS];
uniform int numLights;

uniform vec3 viewPos;

// shadows
uniform sampler2D shadowMap;
uniform mat4      lightSpaceMatrix;
uniform bool      shadowsEnabled;

// settings
uniform float shininess = 32.0;

// texture
vec3 SampleDiffuse()
{
    if (useColor)
        return material.color;

    return texture(material.texture_diffuse1, TexCoords * tiling).rgb;
}

vec3 SampleSpecular()
{
    if (useColor)
        return vec3(0.3);

    return texture(material.texture_specular1, TexCoords * tiling).rgb;
}

// shadow
float ShadowCalculation(vec3 normal, vec3 lightDir)
{
    vec4 fragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);

    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if (projCoords.z > 1.0 ||
        projCoords.x < 0.0 || projCoords.x > 1.0 ||
        projCoords.y < 0.0 || projCoords.y > 1.0)
        return 0.0;

    float currentDepth = projCoords.z;

    float cosTheta = max(dot(normal, lightDir), 0.0);
    float bias = max(0.005 * (1.0 - cosTheta), 0.0005);

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

    for (int x = -1; x <= 1; ++x)
        for (int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += (currentDepth - bias > pcfDepth) ? 1.0 : 0.0;
        }

    return shadow / 9.0;
}

// lighting
vec3 CalcDirectional(Light light, vec3 normal, vec3 viewDir,
                     vec3 diffuseTex, vec3 specularTex, float shadow)
{
    vec3 lightDir = normalize(-light.direction);

    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);

    vec3 ambient  = light.ambient  * diffuseTex;
    vec3 diffuse  = light.diffuse  * diff * diffuseTex;
    vec3 specular = light.specular * spec * specularTex;

    float lit = 1.0 - shadow;
    return ambient + lit * (diffuse + specular);
}

vec3 CalcPoint(Light light, vec3 normal, vec3 viewDir,
               vec3 diffuseTex, vec3 specularTex)
{
    vec3 lightDir = normalize(light.position - FragPos);

    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);

    float dist = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant +
                              light.linear * dist +
                              light.quadratic * dist * dist);

    vec3 ambient  = light.ambient  * diffuseTex * attenuation;
    vec3 diffuse  = light.diffuse  * diff * diffuseTex * attenuation;
    vec3 specular = light.specular * spec * specularTex * attenuation;

    return ambient + diffuse + specular;
}

vec3 CalcSpot(Light light, vec3 normal, vec3 viewDir,
              vec3 diffuseTex, vec3 specularTex, float shadow)
{
    vec3 lightDir = normalize(light.position - FragPos);

    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.innerCutoff - light.outerCutoff;
    float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0);

    if (intensity <= 0.0)
        return vec3(0.0);

    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);

    float dist = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant +
                              light.linear * dist +
                              light.quadratic * dist * dist);

    vec3 ambient  = light.ambient  * diffuseTex * attenuation;
    vec3 diffuse  = light.diffuse  * diff * diffuseTex * attenuation * intensity;
    vec3 specular = light.specular * spec * specularTex * attenuation * intensity;

    float lit = 1.0 - shadow;
    return ambient + lit * (diffuse + specular);
}

// main
void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 diffuseTex = SampleDiffuse();
    vec3 specularTex = SampleSpecular();

    vec3 result = vec3(0.0);

    for (int i = 0; i < numLights; ++i)
    {
        float shadow = 0.0;

        if (shadowsEnabled && (lights[i].type == 0 || lights[i].type == 2))
        {
            vec3 lightDir = (lights[i].type == 0)
                ? normalize(-lights[i].direction)
                : normalize(lights[i].position - FragPos);

            shadow = ShadowCalculation(norm, lightDir);
        }

        if (lights[i].type == 0)
            result += CalcDirectional(lights[i], norm, viewDir,
                                     diffuseTex, specularTex, shadow);

        else if (lights[i].type == 1)
            result += CalcPoint(lights[i], norm, viewDir,
                                diffuseTex, specularTex);

        else if (lights[i].type == 2)
            result += CalcSpot(lights[i], norm, viewDir,
                               diffuseTex, specularTex, shadow);
    }

    if (numLights == 0)
        result = diffuseTex * 0.3;

    FragColor = vec4(result, 1.0);
}