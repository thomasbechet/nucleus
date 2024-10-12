#version 330 core

layout(location = 0) in vec3 in_position;

uniform uvec2 viewport_size;
uniform mat4 model;
uniform mat4 view_projection;

vec4 snap_vertex(in vec4 position)
{
    vec2 grid = vec2(viewport_size);
    position.xy = position.xy / position.w;
    position.xy = (floor(grid * position.xy) + 0.5) / grid;
    position.xy *= position.w;
    return position;
}

void main()
{
    vec4 position = view_projection * model * vec4(in_position, 1);
    gl_Position = snap_vertex(position);
}
