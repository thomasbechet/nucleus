#ifndef NUGL_MESH_H
#define NUGL_MESH_H

#include <nucleus/core/core.h>
#include <nucleus/external/glad/gl.h>

#define NUGL__VERTEX_SIZE (3 + 2 + 3)

typedef struct
{
    GLuint    vao;
    GLuint    vbo;
    nu_size_t vertex_count;
} nugl__mesh_t;

typedef NU_VEC(nugl__mesh_t) nugl__mesh_vec_t;

#endif
