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

float sampleShadowMap(sampler2D depthMap, vec2 coords, float lod, float compare)
{
    return step(compare, texture(depthMap, coords, lod).r);
}

float sampleShadowMapLinear(sampler2D depthMap, vec2 coords, float compare)
{
    const int lod = 0;
    vec2 texelSize = 1.0 / textureSize(depthMap, lod);
    vec2 pixelPos = coords.xy / texelSize + vec2(0.5);
    vec2 fracPart = fract(pixelPos);
    vec2 startTexel = (pixelPos - fracPart) * texelSize;

    vec4 samples;
    samples[0] = sampleShadowMap(depthMap, startTexel + vec2(0.0, 0.0) * texelSize, lod, compare);
    samples[1] = sampleShadowMap(depthMap, startTexel + vec2(1.0, 0.0) * texelSize, lod, compare);
    samples[2] = sampleShadowMap(depthMap, startTexel + vec2(0.0, 1.0) * texelSize, lod, compare);
    samples[3] = sampleShadowMap(depthMap, startTexel + vec2(1.0, 1.0) * texelSize, lod, compare);

    float sampleA = mix(samples[0], samples[2], fracPart.y);
    float sampleB = mix(samples[1], samples[3], fracPart.y);

    return mix(sampleA, sampleB, fracPart.x);
}

float ShadowCalculation()
{
    vec3 projCoords = _fragPosLightSpace.xyz / _fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    vec3 normal = normalize(_normal);
    vec3 lightDir = normalize(-lightDir);
    float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.0005);
    //float bias = 0.005;
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;
    if(projCoords.z > 1.0)
        shadow = 0.0;

    return (1 - shadow);
}

float ShadowCalculation2()
{
    vec3 projCoords = _fragPosLightSpace.xyz / _fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    float bias = max(0.005 * (1.0 - dot(_normal, lightDir)), 0.0005);
    const int lod = 0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, lod);
    const float NUM_SAMPLES = 3.0f;
    const float SAMPLES_START = (NUM_SAMPLES-1.0f)/2.0f;
    const float NUM_SAMPLES_SQUARED = NUM_SAMPLES*NUM_SAMPLES;

    float shadow = 0.0f;
    for(float y = -SAMPLES_START; y <= SAMPLES_START; y += 1.0f)
    {
        for(float x = -SAMPLES_START; x <= SAMPLES_START; x += 1.0f)
        {
            vec2 coordsOffset = vec2(x,y) * texelSize;
            float pcfDepth = sampleShadowMapLinear(shadowMap, projCoords.xy + coordsOffset, currentDepth);
            shadow += currentDepth -  - bias > pcfDepth ? 0.0 : 1.0;
        }
    }

    shadow /= NUM_SAMPLES_SQUARED;

    if(projCoords.z > 1.0)
        shadow = 1.0;

    return shadow;
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

    /*vec3 projCoords = _fragPosLightSpace.xyz / _fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float currentDepth = projCoords.z;
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float shadow = sampleShadowMapLinear(shadowMap, projCoords.xy, currentDepth);

    float bias = max(0.005 * (1.0 - dot(_normal, lightDir)), 0.0005);
    shadow = currentDepth - bias > closestDepth  ? 0.0 : 1.0;

    if(projCoords.z > 1.0)
        shadow = 1.0;*/


    color = (vec4(ambientLighting, 1) + vec4(diffuse, 1) * shadow) * texture(u_texture, pass_texCoord);
    //color = texture(shadowMap, pass_texCoord);
    //color = vec4(shadow, 0.0, 0.0, 0.0);
    //color = vec4(lightDir, 0.0);
}
