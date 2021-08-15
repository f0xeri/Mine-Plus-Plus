#version 420

layout (location = 0) in vec3 position;

uniform mat4 model;
uniform mat4 projView;

void main() {
    gl_Position = projView * model * vec4(position, 1);
}
