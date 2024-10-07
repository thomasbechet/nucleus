#ifndef NUGL_TEXTURE_H
#define NUGL_TEXTURE_H

#include <nucleus/graphics/graphics.h>
#include <nucleus/external/glad/gl.h>

typedef struct
{
    nu_vec2u_t        size;
    GLuint            texture;
    nu_texture_type_t usage;
} nugl__texture_t;

typedef struct
{
    GLuint texture;
} nugl__cubemap_t;

typedef NU_VEC(nugl__texture_t) nugl__texture_vec_t;
typedef NU_VEC(nugl__cubemap_t) nugl__cubemap_vec_t;

#endif
