#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in vec3 fragPos;
in vec2 pass_texCoord;
in vec3 _normal;

uniform sampler2D u_texture;

void main()
{
    // store the fragment position vector in the first gbuffer texture
    gPosition = fragPos;
    // also store the per-fragment normals into the gbuffer
    gNormal = normalize(_normal);
    // and the diffuse per-fragment color
    gAlbedoSpec.rgb = texture(u_texture, pass_texCoord).rgb;
}