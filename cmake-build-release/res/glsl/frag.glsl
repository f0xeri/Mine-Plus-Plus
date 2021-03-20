#version 420

out vec4 color;

in vec3 fragPos;
in vec3 _normal;
in vec2 pass_texCoord;
in vec4 _fragPosLightSpace;

layout(binding = 0) uniform sampler2D u_texture;
layout(binding = 1) uniform sampler2D shadowMap;
uniform vec3 lightDir;
uniform vec3 viewPos;

float ShadowCalculation()
{
    vec3 projCoords = _fragPosLightSpace.xyz / _fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    vec3 normal = normalize(_normal);
    vec3 lightDir = normalize(-lightDir);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;
    if(projCoords.z > 1.0)
        shadow = 0.0;

    return (1 - shadow);
}

void main()
{
    vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
    float ambientStrength = 0.4f;
    vec3 ambientLighting = ambientStrength * lightColor;
    vec3 norm = normalize(_normal);
    vec3 lightDir = normalize(-lightDir);
    float diff = max(dot(norm, lightDir), 0.0f);
    vec3 diffuse = diff * lightColor * 0.8f;
    //color = (vec4(ambientLighting, 1) + vec4(diffuse, 1)) * texture(u_texture, pass_texCoord);
    float shadow = ShadowCalculation();
    color = (vec4(ambientLighting, 1) + vec4(diffuse, 1) * shadow) * texture(u_texture, pass_texCoord);
    //color = texture(shadowMap, pass_texCoord);
    //color = vec4(shadow, 0.0, 0.0, 0.0);
    //color = vec4(lightDir, 0.0);
}
