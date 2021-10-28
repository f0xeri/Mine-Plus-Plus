#version 420

out vec4 fragColor;

in vec2 pass_texCoord;
/*in vec4 _fragPosLightSpace;
in mat4 _lightProjection;
in mat4 _lightSpaceMatrix;
in mat4 _lightViewMatrix;*/

layout (std140, binding = 0) uniform LightSpaceMatrices
{
    mat4 lightSpaceMatrices[16];
};

layout(binding = 0) uniform sampler2D gPosition;
layout(binding = 1) uniform sampler2D gNormal;
layout(binding = 2) uniform sampler2D gAlbedoSpec;
layout(binding = 3) uniform sampler2D gDepth;
layout(binding = 4) uniform sampler2DArray shadowMap;

uniform float cascadePlaneDistances[16];
uniform int cascadeCount;
uniform mat4 lightViewMatrices[16];
uniform mat4 lightProjMatrices[16];

uniform vec3 lightDir;
uniform vec3 viewPos;
uniform bool useShadows;
uniform float interpolationX;
uniform vec3 lightPos;
uniform float time;
uniform float farPlane;

const vec2 poissonDisk[64] = {
vec2(-0.04117257f, -0.1597612f),
vec2(0.06731031f, -0.4353096f),
vec2(-0.206701f, -0.4089882f),
vec2(0.1857469f, -0.2327659f),
vec2(-0.2757695f, -0.159873f),
vec2(-0.2301117f, 0.1232693f),
vec2(0.05028719f, 0.1034883f),
vec2(0.236303f, 0.03379251f),
vec2(0.1467563f, 0.364028f),
vec2(0.516759f, 0.2052845f),
vec2(0.2962668f, 0.2430771f),
vec2(0.3650614f, -0.1689287f),
vec2(0.5764466f, -0.07092822f),
vec2(-0.5563748f, -0.4662297f),
vec2(-0.3765517f, -0.5552908f),
vec2(-0.4642121f, -0.157941f),
vec2(-0.2322291f, -0.7013807f),
vec2(-0.05415121f, -0.6379291f),
vec2(-0.7140947f, -0.6341782f),
vec2(-0.4819134f, -0.7250231f),
vec2(-0.7627537f, -0.3445934f),
vec2(-0.7032605f, -0.13733f),
vec2(0.8593938f, 0.3171682f),
vec2(0.5223953f, 0.5575764f),
vec2(0.7710021f, 0.1543127f),
vec2(0.6919019f, 0.4536686f),
vec2(0.3192437f, 0.4512939f),
vec2(0.1861187f, 0.595188f),
vec2(0.6516209f, -0.3997115f),
vec2(0.8065675f, -0.1330092f),
vec2(0.3163648f, 0.7357415f),
vec2(0.5485036f, 0.8288581f),
vec2(-0.2023022f, -0.9551743f),
vec2(0.165668f, -0.6428169f),
vec2(0.2866438f, -0.5012833f),
vec2(-0.5582264f, 0.2904861f),
vec2(-0.2522391f, 0.401359f),
vec2(-0.428396f, 0.1072979f),
vec2(-0.06261792f, 0.3012581f),
vec2(0.08908027f, -0.8632499f),
vec2(0.9636437f, 0.05915006f),
vec2(0.8639213f, -0.309005f),
vec2(-0.03422072f, 0.6843638f),
vec2(-0.3734946f, -0.8823979f),
vec2(-0.3939881f, 0.6955767f),
vec2(-0.4499089f, 0.4563405f),
vec2(0.07500362f, 0.9114207f),
vec2(-0.9658601f, -0.1423837f),
vec2(-0.7199838f, 0.4981934f),
vec2(-0.8982374f, 0.2422346f),
vec2(-0.8048639f, 0.01885651f),
vec2(-0.8975322f, 0.4377489f),
vec2(-0.7135055f, 0.1895568f),
vec2(0.4507209f, -0.3764598f),
vec2(-0.395958f, -0.3309633f),
vec2(-0.6084799f, 0.02532744f),
vec2(-0.2037191f, 0.5817568f),
vec2(0.4493394f, -0.6441184f),
vec2(0.3147424f, -0.7852007f),
vec2(-0.5738106f, 0.6372389f),
vec2(0.5161195f, -0.8321754f),
vec2(0.6553722f, -0.6201068f),
vec2(-0.2554315f, 0.8326268f),
vec2(-0.5080366f, 0.8539945f)
};


/*float ShadowCalculationHard()
{
    vec3 projCoords = _fragPosLightSpace.xyz / _fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    vec3 normal = texture(gNormal, pass_texCoord).rgb;
    vec3 lightDir = normalize(-lightDir);
    float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.0005);
    //float bias = 0.005;

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;
    if (projCoords.z > 1.0)
    shadow = 0.0;

    return (1 - shadow);
}*/

float rand(vec4 co)
{
    float dot_product = dot(co, vec4(12.9898, 78.233, 45.164, 94.673));
    return fract(sin(dot_product) * 43758.5453);
}

float Shadow(sampler2DArray ShadowMapTex, vec3 projCoords, float bias, int layer)
{
    float closestDepth = texture(ShadowMapTex, vec3(projCoords.xy, layer)).r;
    float currentDepth = projCoords.z;

    return currentDepth - bias < closestDepth ? 1.0 : 0.0;
}

float ShadowCalculationSoft(vec3 fragPos)
{
    float depthValue = texture(gDepth, pass_texCoord).z;
    float z_n = 2.0 * depthValue - 1.0;
    float zNear = 0.1f;
    float zFar = 500.0f;
    float depthWorld = 2.0 * zNear * zFar / (zFar + zNear - z_n * (zFar - zNear));

    int layer = -1;
    for (int i = 0; i < cascadeCount; ++i)
    {
        if (depthWorld < cascadePlaneDistances[i])
        {
            layer = i;
            break;
        }
    }
    if (layer == -1)
    {
        layer = cascadeCount;
    }

    vec4 fragPosLightSpace = lightSpaceMatrices[layer] * vec4(fragPos, 1.0);

    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if (projCoords.z > 1.0)
    return 1;

    vec2 texCoord = projCoords.xy;
    float zReceiver = projCoords.z;
    float filterRadius = 0.003;

    vec3 normal = texture(gNormal, pass_texCoord).rgb;
    vec3 lightDir = normalize(-lightDir);
    float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.0005);
    /*if (layer == cascadeCount)
    {
        bias *= 1 / (farPlane * 0.5f);
    }
    else
    {
        bias *= 1 / (cascadePlaneDistances[layer] * 0.5f);
    }*/
    //bias = 0.005;
    float sum = 0;
    float theta = rand(vec4(texCoord, gl_FragCoord.xy));
    mat2 rotation = mat2(vec2(cos(theta), sin(theta)), vec2(-sin(theta), cos(theta)));
    vec2 texelSize = 1.0 / vec2(textureSize(shadowMap, 0));
    for (int i = 0; i < 9; i++)
    {
        vec2 offset = (rotation * poissonDisk[i]) * filterRadius;
        vec2 texOffset = texCoord + offset;
        bvec2 outside = greaterThan(texOffset, vec2(1.0, 1.0));
        bvec2 inside = lessThan(texOffset, vec2(0.0, 0.0));
        float ishadow = Shadow(shadowMap, vec3(texOffset, zReceiver), bias, layer);
        sum += ((any(outside) || any(inside)) ? 1.0f : ishadow);
    }

    sum /= 9;

    return sum;
}


float _normalize(float val, float min, float max) {
    return (val - min) / (max - min);
}

float dither_pattern[16] = float[16] (
    0.0f, 0.5f, 0.125f, 0.625f,
    0.75f, 0.22f, 0.875f, 0.375f,
    0.1875f, 0.6875f, 0.0625f, 0.5625,
    0.9375f, 0.4375f, 0.8125f, 0.3125);

#define PI_RCP (0.31830988618379067153776752674503)

float tri( float x ){
    return abs( fract(x) - .5 );
}

vec3 tri3( vec3 p ){

    return vec3(
    tri( p.z + tri( p.y * 1. ) ),
    tri( p.z + tri( p.x * 1. ) ),
    tri( p.y + tri( p.x * 1. ) )
    );

}

// Taken from https://www.shadertoy.com/view/4ts3z2
// By NIMITZ  (twitter: @stormoid)
float triNoise3d(in vec3 p, in float spd, in float time)
{
    float z=1.4;
    float rz = 0.;
    vec3 bp = p;
    for (float i=0.; i<=3.; i++ )
    {
        vec3 dg = tri3(bp*2.);
        p += (dg+time*spd);

        bp *= 1.8;
        z *= 1.5;
        p *= 1.2;
        //p.xz*= m2;

        rz+= (tri(p.z+tri(p.x+tri(p.y))))/z;
        bp += 0.14;
    }
    return rz;
}

float sample_fog(vec3 pos) {
    return triNoise3d(pos * 2.2 / 8, 0.2, time)*0.75;
}

void main()
{
    vec3 fragPos = texture(gPosition, pass_texCoord).rgb;
    vec3 normal = texture(gNormal, pass_texCoord).rgb;
    float x0 = interpolationX;
    float x1 = -interpolationX;
    vec3 fx0 = vec3(0.0, 0.0, 0.0);
    vec3 fx1 = vec3(0.5, 0.8, 1.0);

    float p = _normalize(lightDir.y, x1, x0);

    vec3 res = mix(fx0, fx1, p);

    if ((normal.x == 0.0) && (normal.y == 0.0) && (normal.z == 0.0))
    {
        fragColor = vec4(res, 1.0);
    }
    else
    {
        vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
        float ambientStrength = 0.4f;
        vec3 ambientLighting = ambientStrength * lightColor;
        vec3 norm = normalize(normal);
        vec3 lightDir = normalize(-lightDir);
        float diff = max(dot(norm, lightDir), 0.0f);
        vec3 diffuse = diff * lightColor * 0.8f;

        float shadow = 1.0;

        if (useShadows) shadow = ShadowCalculationSoft(fragPos);


        fragColor = (vec4(ambientLighting, 1) + vec4(diffuse, 1) * shadow) * texture(gAlbedoSpec, pass_texCoord);
        //fragColor = vec4(_normalize(layer, 0, cascadeCount));
        //fragColor = vec4(_normalize(10, cascadePlaneDistances[0], cascadePlaneDistances[cascadeCount]));
        //fragColor = vec4(depthValue);
    }
}


/*void main2()
{
    bool hasFog = false;
    float dither_value = dither_pattern[ (int(gl_FragCoord.x) % 4) * 4 + (int(gl_FragCoord.y) % 4) ];
    int lightNumSamples = 16;
    vec3 fragPos = texture(gPosition, pass_texCoord).rgb;
    vec4 fragPosLightSpace = _lightSpaceMatrix * vec4(fragPos, 1.0);
    vec3 normal = texture(gNormal, pass_texCoord).rgb;
    mat4 lightViewMatrix = _lightViewMatrix;

    vec4 end_pos_worldspace  = vec4(viewPos, 1.0);
    vec4 start_pos_worldspace = vec4(fragPos, 1.0);
    vec4 delta_worldspace = normalize(end_pos_worldspace - start_pos_worldspace);

    vec4 end_pos_lightview = lightViewMatrix * end_pos_worldspace;
    vec4 start_pos_lightview = lightViewMatrix * start_pos_worldspace;
    vec4 delta_lightview = normalize(end_pos_lightview - start_pos_lightview);

    float raymarch_distance_lightview = length(end_pos_lightview - start_pos_lightview);
    float step_size_lightview = raymarch_distance_lightview / lightNumSamples;

    float raymarch_distance_worldspace = length(end_pos_worldspace - start_pos_worldspace);
    float step_size_worldspace = raymarch_distance_worldspace / lightNumSamples;

    vec4 ray_position_lightview = start_pos_lightview + dither_value*step_size_lightview * delta_lightview;
    vec4 ray_position_worldspace = start_pos_worldspace + dither_value*step_size_worldspace * delta_worldspace;

    float light_contribution = 0.0;
    float phi = 20000.0;
    float tau = 0.05;

    if ((normal.x == 0.0) && (normal.y == 0.0) && (normal.z == 0.0))
    {
        float x0 = interpolationX;
        float x1 = -interpolationX;
        vec3 fx0 = vec3(0.0, 0.0, 0.0);
        vec3 fx1 = vec3(0.5, 0.8, 1.0);

        float p = _normalize(lightDir.y, x1, x0);

        vec3 res = mix(fx0, fx1, p);

        fragColor = vec4(res, 1.0);
    }
    else
    {
        for (float l = raymarch_distance_worldspace; l >= step_size_worldspace; l -= step_size_worldspace) {
            vec4 ray_position_lightspace = _lightProjection * vec4(ray_position_lightview.xyz, 1);
            // perform perspective divide
            vec3 proj_coords = ray_position_lightspace.xyz / ray_position_lightspace.w;

            // transform to [0,1] range
            proj_coords = proj_coords * 0.5 + 0.5;

            vec4 closest_depth = texture(shadowMap, proj_coords.xy);
            //float shadow_term = 1.0;
            //if (proj_coords.z - 0.0005 > closest_depth.r) {
                //shadow_term = 0.0;
            //}

            float shadow_term = ShadowCalculationSoft(ray_position_lightspace);

            float d = length(ray_position_worldspace.xyz - lightPos);
            float d_rcp = 1.0/d;

            float fog = hasFog ? sample_fog(ray_position_worldspace.xyz) : 1.0;

            light_contribution += fog * tau * (shadow_term * (phi * 0.25 * PI_RCP) * d_rcp * d_rcp ) * exp(-d*tau)*exp(-l*tau) * step_size_worldspace;

            ray_position_lightview += step_size_lightview * delta_lightview;
            ray_position_worldspace += step_size_worldspace * delta_worldspace;
        }

        vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
        float ambientStrength = 0.6f;
        vec3 ambientLighting = ambientStrength * lightColor;
        vec3 norm = normalize(normal);
        vec3 lightDir = normalize(-lightDir);
        float diff = max(dot(norm, lightDir), 0.0f);
        vec3 diffuse = diff * lightColor * 0.8f;

        fragColor.xyz = ambientLighting * min(light_contribution, 1.0);
        fragColor *= texture(gAlbedoSpec, pass_texCoord);
        //fragColor.xyz = vec3(light_contribution);
    }
}*/
