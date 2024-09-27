#version 330 core

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec2 in_uv;

uniform uvec2 viewport_size;
uniform mat4 model;
uniform mat4 view_projection;
uniform mat3 uv_transform;

uniform sampler2D texture0;

out vec2 uv;

void main()
{
    // Transform vertex
    vec4 position = view_projection * model * vec4(in_position, 1);

    // Prevent sub pixel aliasing by flooring vertices to pixel
    // vec2 grid = vec2(viewport_size);
    // position.xyz = position.xyz / position.w;
    // position.xy = floor(grid * position.xy) / grid;
    // position.xyz *= position.w;

    gl_Position = position;

    // Floors UVs
    uv = (uv_transform * vec3(in_uv, 1)).xy;
    // ivec2 tex_size = textureSize(texture0, 0);
    // uv = (floor(tex_size * uv) + 0.5) / tex_size;
}
