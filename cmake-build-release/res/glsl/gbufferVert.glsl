#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 normal;

out vec3 fragPos;
out vec2 pass_texCoord;
out vec3 _normal;

uniform mat4 model;
uniform mat4 projView;

void main()
{
    vec4 worldPos = model * vec4(position, 1.0);
    fragPos = worldPos.xyz;
    pass_texCoord = texCoord;

    mat3 normalMatrix = transpose(inverse(mat3(model)));
    _normal = normalMatrix * normal;

    gl_Position = projView * model * vec4(position, 1);
}