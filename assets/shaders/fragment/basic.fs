#version 330 core

out vec4 FragColor;

in VS_OUT 
{
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

struct Material 
{
    vec3 color;

    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    sampler2D texture_normal1;
    sampler2D texture_height1;
};

uniform Material material;

uniform bool useColor;

// uniform vec3 viewPos;
// uniform vec3 lightPos;

void main()
{
    vec3 baseColor;
    
    if (useColor) 
        baseColor = material.color;
    else 
        baseColor = texture(material.texture_diffuse1, fs_in.TexCoords).rgb;
    
    vec3 ambient = 0.6 * baseColor;
    
    vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0)); 
    vec3 norm = normalize(fs_in.Normal);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * baseColor * 0.4;
    
    vec3 result = ambient + diffuse;
    FragColor = vec4(result, 1.0);
}