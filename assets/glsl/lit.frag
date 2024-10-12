#version 330 core

out vec4 frag_color;

uniform sampler2D texture0;
uniform sampler2D shadow_map;

in VS_OUT {
    vec2 uv;
    vec3 normal;
    vec3 frag_pos;
    vec4 frag_pos_shadow;
} fs_in;

float shadow_calculation(vec4 pos)
{
    vec3 proj_coords = (pos.xyz / pos.w) * 0.5 + 0.5;
    if (proj_coords.z > 1.0) return 0.0;
    float depth = texture(shadow_map, proj_coords.xy).r;
    const float bias = 0.005;
    return proj_coords.z - bias > depth ? 1 : 0;
}

void main()
{
    float shadow = shadow_calculation(fs_in.frag_pos_shadow);

    vec4 color = texture(texture0, fs_in.uv);
    color *= max(0.0, dot(fs_in.normal, normalize(vec3(1))));
    frag_color = color * (1.0 - shadow);
}
