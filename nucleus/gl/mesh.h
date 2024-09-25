#ifndef NUGL_MESH_H
#define NUGL_MESH_H

#include <nucleus/graphics/api.h>
#include <nucleus/external/glad/gl.h>

typedef struct
{
    GLuint         vao;
    nu_size_t      capacity;
    nu_primitive_t primitive;
    GLuint         positions;
    GLuint         uvs;
    GLuint         normals;
} nugl__mesh_t;

typedef NU_VEC(nugl__mesh_t) nugl__mesh_vec_t;

#endif
