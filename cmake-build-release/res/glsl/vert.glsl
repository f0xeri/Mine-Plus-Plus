#version 460

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;

// out vec4 pass_color;
out vec2 pass_texCoord;

uniform mat4 model;
uniform mat4 projView;

void main()
{
    gl_Position = projView * model * vec4(position, 1);
    pass_texCoord = vec2(texCoord.x, texCoord.y);
    // pass_color = vec4(light, light, light, 1.0f);
}