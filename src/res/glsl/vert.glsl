#version 460

in vec3 position;
in vec3 color;

out vec3 pass_color;

void main()
{
    gl_Position = vec4(position, 1);
    pass_color = color;
}
