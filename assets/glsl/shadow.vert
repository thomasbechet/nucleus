#version 330 core

layout(location = 0) in vec3 in_position;

uniform mat4 model;
uniform mat4 view_projection;

void main()
{
    vec4 position = view_projection * model * vec4(in_position, 1);
    gl_Position = position;
}
