#version 330 core

out vec2 uv;

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec2 in_uv;

uniform mat4 model;
uniform mat4 view_projection;

void main()
{
    uv = in_uv;
    gl_Position = view_projection * model * vec4(in_position, 1.0);
}
