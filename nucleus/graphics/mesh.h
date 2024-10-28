#ifndef NU_MESH_H
#define NU_MESH_H

#include <nucleus/graphics/backend.h>

typedef struct
{
#ifdef NU_BUILD_GRAPHICS_GL
    nugl__mesh_t gl;
#endif
    nu_size_t      capacity;
    nu_primitive_t primitive;
} nu__mesh_t;

typedef NU_POOL(nu__mesh_t) nu__mesh_pool_t;

#endif
