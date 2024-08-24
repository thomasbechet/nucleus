#ifndef NU_MESH_H
#define NU_MESH_H

#include <nucleus/core.h>

typedef struct
{
    const nu_vec3_t *positions;
    const nu_vec2_t *uvs;
    const nu_vec3_t *normals;
    nu_size_t        count;
} nu_mesh_info_t;

typedef union
{
#ifdef NU_BUILD_RENDERER_GL
    nugl__handle_t _gl;
#endif
} nu_mesh_handle_t;

#endif
