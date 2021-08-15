#version 420

out vec4 color;
in vec2 pass_texCoord;

layout(binding = 0) uniform sampler2D u_texture;

void main() {
    //color = texture(u_texture, pass_texCoord);
    color = normalize(vec4(255, 219, 172, 0));
}
