#ifndef NU_TEXTURE_H
#define NU_TEXTURE_H

#include <nucleus/graphics/api.h>
#include <nucleus/graphics/backend.h>

typedef struct
{
#ifdef NU_BUILD_GRAPHICS_GL
    nugl__texture_t gl;
#endif
    nu_texture_type_t type;
    nu_v3u_t          size;
    nu_size_t         layer;
    nu_image_t        image_texture;
} nu__texture_t;

#endif
