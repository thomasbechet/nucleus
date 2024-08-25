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

NU_DEFINE_HANDLE(nu_mesh_t);

#endif
