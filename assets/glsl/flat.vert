#version 330 core

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec2 in_uv;

uniform mat4 model;
uniform mat4 view_projection;
uniform mat3 uv_transform;

out vec2 uv;

void main()
{
    vec4 position = view_projection * model * vec4(in_position, 1);

    vec2 grid = vec2(640, 400);
    position.xyz = position.xyz / position.w;
    position.xy = floor(grid * position.xy) / grid;
    position.xyz *= position.w;

    gl_Position = position;

    uv = (uv_transform * vec3(in_uv, 1)).xy;
}
