#version 330 core

in vec2 uv;
in vec3 normal;
out vec4 frag_color;

uniform sampler2D texture0;

void main()
{
    vec4 color = texture(texture0, uv);
    float d = dot(normal, normalize(vec3(1)));
    d = (d + 1) * 0.5;
    color *= max(0.5, dot(normal, normalize(vec3(1))));
    frag_color = color * d;
}
