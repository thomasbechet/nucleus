#ifndef NU_CUBEMAP_H
#define NU_CUBEMAP_H

#include <nucleus/graphics/texture.h>

typedef struct
{
    nu_u32_t           size;
    nu_texture_usage_t usage;
    const nu_color_t  *colors_posx;
    const nu_color_t  *colors_negx;
    const nu_color_t  *colors_posy;
    const nu_color_t  *colors_negy;
    const nu_color_t  *colors_posz;
    const nu_color_t  *colors_negz;
} nu_cubemap_info_t;

#define NU_CUBEMAP_INFO_DEFAULT \
    (nu_cubemap_info_t)         \
    {                           \
        .info = { 0 }           \
    }

NU_DEFINE_HANDLE(nu_cubemap_handle_t);

#endif
