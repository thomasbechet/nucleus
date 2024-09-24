#ifndef NU_MODEL_H
#define NU_MODEL_H

#include <nucleus/graphics/api.h>

typedef struct
{
    nu_u16_t  mesh;
    nu_u16_t  material;
    nu_mat4_t transform;
} nu__model_node_t;

typedef union
{
    struct
    {
        nu_mesh_t handle;
        nu_size_t count;
    } mesh;
    nu_texture_t  texture;
    nu_material_t material;
} nu__model_resource_t;

typedef NU_VEC(nu__model_resource_t) nu__model_resource_vec_t;
typedef NU_VEC(nu__model_node_t) nu__model_node_vec_t;

typedef struct
{
    nu_allocator_t           allocator;
    nu__model_resource_vec_t resources;
    nu__model_node_vec_t     nodes;
} nu__model_t;

typedef NU_POOL(nu__model_t) nu__model_pool_t;

#endif
