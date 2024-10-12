#version 330 core

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec2 in_uv;
layout(location = 2) in vec3 in_normal;

uniform uvec2 viewport_size;
uniform mat4 model;
uniform mat4 view_projection;
uniform mat3 uv_transform;
uniform mat4 shadowmap_view_proj;

out VS_OUT {
    vec2 uv;
    vec3 normal;
    vec3 frag_pos;
    vec4 frag_pos_light_space;
} vs_out;

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
    vs_out.frag_pos = vec3(model * vec4(in_position, 1));

    vs_out.uv = (uv_transform * vec3(in_uv, 1)).xy;

    vs_out.normal = mat3(transpose(inverse(model))) * in_normal;

    vs_out.frag_pos_light_space = shadowmap_view_proj * vec4(vs_out.frag_pos, 1);

    vec4 position = view_projection * vec4(vs_out.frag_pos, 1);
    gl_Position = snap_vertex(position);
}
