#ifndef NU_IMAGE_H
#define NU_IMAGE_H

#include <nucleus/core/math.h>

typedef union
{
    struct
    {
        nu_u8_t r;
        nu_u8_t g;
        nu_u8_t b;
        nu_u8_t a;
    };
    nu_u32_t rgba;
} nu_color_t;

NU_API nu_color_t nu_color(nu_u8_t r, nu_u8_t g, nu_u8_t b, nu_u8_t a);
NU_API nu_vec4_t  nu_color_to_vec4(nu_color_t c);

#endif
