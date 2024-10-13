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
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadow_map, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadow_map, proj_coords.xy + vec2(x, y) * texelSize).r; 
            shadow += proj_coords.z - bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    return shadow;
}

void main()
{
    float shadow = shadow_calculation(fs_in.frag_pos_shadow);
    vec4 color = texture(texture0, fs_in.uv);
    color *= max(0.0, dot(fs_in.normal, normalize(vec3(1))));
    frag_color = color * (1.0 - shadow);
}
