#version 330 core

out vec4 out_color;
in vec2 uv;

uniform sampler2D t_surface;

vec2 uv_filtering(in vec2 uv, in vec2 texture_size)
{
    vec2 pixel = uv * texture_size;
    vec2 seam = floor(pixel + 0.5);
    vec2 dudv = fwidth(pixel);
    vec2 rel = (pixel - seam) / dudv;
    vec2 mid_pix = vec2(0.5);
    pixel = seam + clamp(rel, -mid_pix, mid_pix);
    return pixel / texture_size;
}

void main()
{
    vec2 filtered_uv = uv_filtering(uv, textureSize(t_surface, 0));
    out_color = texture(t_surface, filtered_uv);
}
