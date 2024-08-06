#version 330 core

in vec2 uv;
in vec3 normal;
out vec4 color;

uniform sampler2D texture0;

void main()
{
    color = texture(texture0, uv);
    color *= max(0, dot(normal, normalize(vec3(1))));
    color += vec4(1) * 0.05;
}
