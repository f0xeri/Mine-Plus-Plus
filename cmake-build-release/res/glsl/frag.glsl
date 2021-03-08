#version 460

out vec4 color;

in vec3 fragPos;
in vec3 _normal;
in vec2 pass_texCoord;

layout(binding = 0) uniform sampler2D u_texture;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
    vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
    float ambientStrength = 0.40f;
    vec3 ambientLighting = ambientStrength * lightColor;
    vec3 norm = normalize(_normal);
    vec3 lightDirection = normalize(lightPos - fragPos);
    float diff = max(dot(norm, lightDirection), 0.0f);
    vec3 diffuse = diff * lightColor * 0.8f;

    color = (vec4(ambientLighting, 1) + vec4(diffuse, 1)) * texture(u_texture, pass_texCoord);
}
