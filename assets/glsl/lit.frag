#version 330 core

in vec2 uv;
in vec3 normal;
out vec4 frag_color;

uniform sampler2D texture0;
uniform sampler2D shadow_map;

void main()
{
    vec4 color = texture(texture0, uv);
    color *= max(0.0, dot(normal, normalize(vec3(1))));
    frag_color = color;
}
