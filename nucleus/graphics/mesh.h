#ifndef NU_MESH_H
#define NU_MESH_H

#include <nucleus/graphics/backend.h>

typedef struct
{
    nugfx_primitive_t primitive;
    nu_size_t      capacity;
    nu_v3_t       *positions;
    nu_v2_t       *uvs;
    nu_color_t    *colors;
    nu_bool_t      dirty;
#ifdef NU_BUILD_GRAPHICS_GL
    nugl__mesh_t gl;
#endif
} nu__mesh_t;

#endif
