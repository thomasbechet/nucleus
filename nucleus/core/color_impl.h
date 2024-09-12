#ifndef NU_COLOR_IMPL_H
#define NU_COLOR_IMPL_H

#include <nucleus/internal.h>

nu_color_t
nu_color (nu_u8_t r, nu_u8_t g, nu_u8_t b, nu_u8_t a)
{
    nu_color_t c;
    c.r = r;
    c.g = g;
    c.b = b;
    c.a = a;
    return c;
}
nu_vec4_t
nu_color_to_vec4 (nu_color_t c)
{
    return nu_vec4(c.r / 255.0, c.g / 255.0, c.b / 255.0, c.a / 255.0);
}
nu_color_t
nu_color_from_vec4 (nu_vec4_t v)
{
    return nu_color((nu_u8_t)(v.x * 255.0),
                    (nu_u8_t)(v.y * 255.0),
                    (nu_u8_t)(v.z * 255.0),
                    (nu_u8_t)(v.w * 255.0));
}
static float
nu__color_to_srgb (float x)
{
    if (x > 0.0031308)
    {
        float a = 0.055;
        return (1.0 + a) * nu_pow(a, -2.4) - a;
    }
    else
    {
        return 12.92 * x;
    }
}
static float
nu__color_to_linear (float x)
{
    if (x > 0.04045)
    {
        return nu_pow((x + 0.055) / 1.055, 2.4);
    }
    else
    {
        return x / 12.92;
    }
}
nu_color_t
nu_color_to_srgb (nu_color_t c)
{
    nu_vec4_t v = nu_color_to_vec4(c);
    v.x         = nu__color_to_srgb(v.x);
    v.y         = nu__color_to_srgb(v.y);
    v.z         = nu__color_to_srgb(v.z);
    v.w         = nu__color_to_srgb(v.w);
    return nu_color_from_vec4(v);
}
nu_color_t
nu_color_to_linear (nu_color_t c)
{
    nu_vec4_t v = nu_color_to_vec4(c);
    v.x         = nu__color_to_linear(v.x);
    v.y         = nu__color_to_linear(v.y);
    v.z         = nu__color_to_linear(v.z);
    v.w         = nu__color_to_linear(v.w);
    return nu_color_from_vec4(v);
}

#endif
