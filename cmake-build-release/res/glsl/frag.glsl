#version 460

out vec4 color;

// in vec4 pass_color;
in vec2 pass_texCoord;
uniform sampler2D u_texture;

void main()
{
    color = texture(u_texture, pass_texCoord);
}
