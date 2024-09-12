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
    nu_mesh_t     mesh;
    nu_texture_t  texture;
    nu_material_t material;
} nu__model_asset_t;

typedef nu_vec(nu__model_asset_t) nu__model_asset_vec_t;
typedef nu_vec(nu__model_node_t) nu__model_node_vec_t;

typedef struct
{
    nu_allocator_t        allocator;
    nu__model_asset_vec_t assets;
    nu__model_node_vec_t  nodes;
} nu__model_t;

typedef nu_pool(nu__model_t) nu__model_pool_t;

#endif
