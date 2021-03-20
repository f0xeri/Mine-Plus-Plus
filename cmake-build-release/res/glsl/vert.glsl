#version 420

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 normal;

out vec2 pass_texCoord;
out vec3 _normal;
out vec3 fragPos;
out vec4 _fragPosLightSpace;

uniform mat4 model;
uniform mat4 projView;
uniform mat4 lightSpaceMatrix;

void main()
{
    fragPos = vec3(model * vec4(position, 1.0));
    _normal = mat3(transpose(inverse(model))) * normal;
    pass_texCoord = texCoord;
    _fragPosLightSpace = lightSpaceMatrix * model * vec4(position, 1.0);
    gl_Position = projView * model * vec4(position, 1);
}