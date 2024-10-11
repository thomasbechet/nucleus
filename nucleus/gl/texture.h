#ifndef NUGL_TEXTURE_H
#define NUGL_TEXTURE_H

#include <nucleus/graphics/graphics.h>
#include <nucleus/external/glad/gl.h>

typedef struct
{
    nu_v3u_t          size;
    GLuint            texture;
    nu_texture_type_t type;
} nugl__texture_t;

typedef NU_VEC(nugl__texture_t) nugl__texture_vec_t;

#endif
