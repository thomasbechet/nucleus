#ifndef NU_MESH_H
#define NU_MESH_H

#include <nucleus/graphics/backend.h>

typedef struct
{
#ifdef NU_BUILD_GRAPHICS_GL
    nugl__mesh_t gl;
#endif
    nu_primitive_t primitive;
    NU_VEC(nu_v3_t) positions;
    NU_VEC(nu_v2_t) uvs;
    struct
    {
        NU_VEC(nu_u16_t) positions;
        NU_VEC(nu_u16_t) uvs;
    } indices;
} nu__mesh_t;

#endif
