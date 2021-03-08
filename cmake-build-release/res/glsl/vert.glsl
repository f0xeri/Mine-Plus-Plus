#version 460

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 normal;

out vec2 pass_texCoord;
out vec3 _normal;
out vec3 fragPos;

uniform mat4 model;
uniform mat4 projView;

void main()
{
    fragPos = vec3(model * vec4(position, 1.0));
    _normal = mat3(transpose(inverse(model))) * normal;
    pass_texCoord = texCoord;
    gl_Position = projView * model * vec4(position, 1);
}