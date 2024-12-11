#ifndef NU_MODEL_H
#define NU_MODEL_H

#include <nucleus/graphics/api.h>

typedef struct
{
    nugfx_mesh_t    mesh;
    nugfx_texture_t texture;
    nu_m4_t         transform;
} nugfx__model_node_t;

typedef struct
{
    NU_ARRAY(nugfx__model_node_t) nodes;
} nugfx__model_t;

#endif
