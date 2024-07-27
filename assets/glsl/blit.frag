#version 330 core

out vec4 color;
in vec2 uv;

uniform sampler2D t_surface;

void main()
{
    color = texture(t_surface, uv);
}
