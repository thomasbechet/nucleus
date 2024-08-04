#version 330 core

out vec2 uv;

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec2 in_uv;

uniform mat4 model;
uniform mat4 view_projection;

void main()
{
    uv = in_uv;

    vec4 position = view_projection * model * vec4(in_position, 1.0);

    vec2 grid = vec2(640 / 2, 400 / 2);
    position.xyz = position.xyz / position.w;
    position.xy = floor(grid * position.xy) / grid;
    position.xyz *= position.w;

    gl_Position = position;
}
