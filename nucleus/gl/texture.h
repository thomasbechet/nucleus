#ifndef NUGL_TEXTURE_H
#define NUGL_TEXTURE_H

#include <nucleus/core/core.h>
#include <nucleus/external/glad/gl.h>

typedef struct
{
    nu_uvec2_t size;
    GLuint     texture;
} nugl__texture_t;

typedef struct
{
    GLuint texture;
} nugl__cubemap_t;

typedef NU_VEC(nugl__texture_t) nugl__texture_vec_t;
typedef NU_VEC(nugl__cubemap_t) nugl__cubemap_vec_t;

#endif
