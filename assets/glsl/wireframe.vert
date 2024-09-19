#version 330 core

layout(location = 0) in vec3 in_position;

uniform uvec2 viewport_size;
uniform mat4 model;
uniform mat4 view_projection;

void main()
{
    // Transform vertex
    vec4 position = view_projection * model * vec4(in_position, 1);

    // Prevent sub pixel aliasing by flooring vertices to pixel
    vec2 grid = vec2(viewport_size);
    position.xyz = position.xyz / position.w;
    position.xy = (floor(grid * position.xy) + 0.5) / grid;
    position.xyz *= position.w;

    gl_Position = position;
}
