#ifndef NUGL_BUFFER_H
#define NUGL_BUFFER_H

#include <nucleus/graphics/api.h>
#include <nucleus/external/glad/gl.h>

typedef struct
{
    nu_buffer_type_t type;
    nu_primitive_t   primitive;
    nu_size_t        size;
    GLuint           vbo;
} nugl__buffer_t;

typedef NU_VEC(nugl__buffer_t) nugl__buffer_vec_t;

#endif
