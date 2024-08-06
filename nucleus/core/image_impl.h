#ifndef NU_IMAGE_IMPL_H
#define NU_IMAGE_IMPL_H

#include <nucleus/core/image.h>

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

#endif
