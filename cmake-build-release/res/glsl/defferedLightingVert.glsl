#version 420

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;

out vec2 pass_texCoord;
out vec4 _fragPosLightSpace;
out mat4 _lightProjection;
out mat4 _lightSpaceMatrix;

uniform mat4 lightSpaceMatrix;
uniform mat4 lightProjection;

void main()
{
    pass_texCoord = texCoord;
    _fragPosLightSpace = lightSpaceMatrix * vec4(position, 1.0);
    _lightProjection = lightProjection;
    _lightSpaceMatrix = lightSpaceMatrix;
    gl_Position = vec4(position, 1);
}