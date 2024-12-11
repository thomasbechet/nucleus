#ifndef NU_TEXTURE_H
#define NU_TEXTURE_H

#include <nucleus/graphics/api.h>
#include <nucleus/graphics/backend.h>

typedef struct
{
#ifdef NU_BUILD_GRAPHICS_GL
    nugl__texture_t gl;
#endif
    nu_v3u_t   size;
    nu_byte_t *data;
} nugfx__texture_t;

#endif
