#version 420

layout (location = 0) in vec2 position;

uniform mat4 model;
uniform mat4 projView;

void main(){
    gl_Position = model * projView * vec4(position, 0.0, 1.0);
}