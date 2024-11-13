#ifndef NU_MODEL_H
#define NU_MODEL_H

#include <nucleus/graphics/api.h>

typedef struct
{
    nu_mesh_t     mesh;
    nu_material_t material;
    nu_m4_t       transform;
} nu__model_node_t;

typedef struct
{
    nu_allocator_t allocator;
    NU_VEC(nu__model_node_t) nodes;
} nu__model_t;

#endif
