#ifndef NUGL_MESH_H
#define NUGL_MESH_H

#include <nucleus/graphics/api.h>
#include <nucleus/external/glad/gl.h>

typedef struct
{
    GLuint         vao;
    nu_primitive_t primitive;
    nu_size_t      count;
    nu_buffer_t    positions;
    nu_size_t      first_position;
    nu_buffer_t    uvs;
    nu_size_t      first_uv;
    nu_buffer_t    normals;
    nu_size_t      first_normal;
} nugl__mesh_t;

typedef NU_VEC(nugl__mesh_t) nugl__mesh_vec_t;

#endif
