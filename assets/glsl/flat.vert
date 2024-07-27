#version 330 core

out vec2 uv;

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec2 in_uv;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(in_position, 1.0);
}
